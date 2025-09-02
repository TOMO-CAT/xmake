#!/bin/bash

set -e

DOCKER_CONTAINER="xmake-dev-container"
DOCKER_HOSTNAME="docker_dev"

PROJECT_BASE_DIR="$(pwd)"
PROJECT_NAME="$(basename ${PROJECT_BASE_DIR})"
DOCKER_IMAGE="${PROJECT_NAME}:latest"

# 获取第一个非回环 IP
# HOST_IP=$(hostname -I 2>/dev/null | awk '{print $1}' | sed 's/ //g')

function info() {
  (>&2 printf "[\e[34m\e[1mINFO\e[0m] $*\n")
}

function error() {
  (>&2 printf "[\033[0;31mERROR\e[0m] $*\n")
}

function warning() {
  (>&2 printf "[\033[0;33mWARNING\e[0m] $*\n")
}

function ok() {
  (>&2 printf "[\e[32m\e[1m OK \e[0m] $*\n")
}

function help_info() {
  local bash_name=$(basename "${BASH_SOURCE[0]}")
  echo "Usage: bash docker.sh COMMAND [OPTIONS]"
  echo ""
  echo "A script to build/run/delete docker container easily"
  echo ""
  echo "Commands:"
  echo "  run              Run container."
  echo "  build            Build container."
  echo "  clear            Delete image && container for this project."
  echo "  help             Print help text."
  echo ""
  echo "[OPTIONS] for docker clear:"
  echo "  --image          Delete docker image."
  echo ""
}



function docker_run() {
  docker exec -it ${DOCKER_CONTAINER} /bin/bash -c "source /home/${USER}/.profile && /bin/bash"
}

function docker_build() {
  if docker images | awk '{print $1":"$2}' | grep -q ${DOCKER_IMAGE}; then
    info "Docker image ${DOCKER_IMAGE} already exists."
  else
    info "Docker image ${DOCKER_IMAGE} does not exist. Start building..."
    docker build --build-arg USER_NAME="${USER}" --progress=plain -t ${DOCKER_IMAGE} .
  fi

  if docker ps -a | grep -q "${DOCKER_CONTAINER}"; then
    info "Docker container ${DOCKER_CONTAINER} already exists."
    exit 0
  fi

  info "Docker container ${DOCKER_CONTAINER} does not exist. Starting..."

  USER_ID=$(id -u)
  GRP=$(id -g -n)
  GRP_ID=$(id -g)
  LOCAL_HOST=$(hostname)
  DOCKER_HOME="/home/$USER"
  [ "$USER" == "root" ] && DOCKER_HOME="/root"

  # 避免 --volume 挂载时文件不存在
  [ ! -f "${HOME}/.gitconfig" ] && touch "${HOME}/.gitconfig"
  # 不挂载 ~/.profile 和 ~/.bashrc 了, 因为如果这里面定义了 CXX CPP CC 和 LD 环境变量会影响交叉编译
  # [ ! -f "${HOME}/.profile" ] && touch "${HOME}/.profile"
  # [ ! -f "${HOME}/.bashrc" ] && touch "${HOME}/.bashrc"
  [ ! -d "${HOME}/.ssh" ] && mkdir "${HOME}/.ssh"

  general_param="-it -d \
    --privileged \
    --restart always \
    --name ${DOCKER_CONTAINER} \
    --env DOCKER_USER=root \
    --env USER=${USER} \
    --env DOCKER_USER_ID=${USER_ID} \
    --env DOCKER_GRP=${GRP} \
    --env DOCKER_GRP_ID=${GRP_ID} \
    --env DOCKER_IMG=${DOCKER_IMAGE} \
    --volume ${PROJECT_BASE_DIR}:/${PROJECT_BASE_DIR} \
    --volume ${HOME}/.gitconfig:${DOCKER_HOME}/.gitconfig \
    --volume ${HOME}/.ssh:${DOCKER_HOME}/.ssh \
    --volume /etc/passwd:/etc/passwd:ro \
    --volume /etc/group:/etc/group:ro \
    --volume /etc/localtime:/etc/localtime:ro \
    --volume /etc/resolv.conf:/etc/resolv.conf:ro \
    --net host \
    --add-host in_dev_docker:127.0.0.1 \
    --add-host ${LOCAL_HOST}:127.0.0.1 \
    --hostname in_dev_docker \
    --workdir ${PROJECT_BASE_DIR}"


  info "Starting docker container \"${DOCKER_CONTAINER}\" ..."

  docker run ${general_param} ${DOCKER_IMAGE} /bin/bash
  ok 'Docker environment has already been setted up, you can enter with cmd: "bash scripts/docker.sh run"'
}

function docker_clear() {
  local docker_clear_image=false
  while [ $# -ge 1 ]; do
    case "$1" in
    --image )
      docker_clear_image=true
      shift 1
      ;;
    * )
      error "Invalid param for docker clear"
      echo ""
      help_info
      exit -1
      ;;
    esac
  done

  info "Stoping docker container \"${DOCKER_CONTAINER}\" ..."
  docker container stop ${DOCKER_CONTAINER} 1>/dev/null || warning "No such container: ${DOCKER_CONTAINER}"
  info "Deleting docker container \"${DOCKER_CONTAINER}\" ..."
  docker container rm -f ${DOCKER_CONTAINER} 1>/dev/null || warning "No such container: ${DOCKER_CONTAINER}"

  if ${docker_clear_image}; then
    info "Deleting docker image \"${DOCKER_IMAGE}\" ..."
    docker rmi ${DOCKER_IMAGE}
  fi
  ok "Delete docker container [${DOCKER_CONTAINER}] successfully!"
}

function main() {
  [ $# -lt 1 ] && {
    echo "Please set param for docker.sh!"
    echo ""
    help_info
    exit -1
  }

  local cmd=$1
  shift 1

  case ${cmd} in
  run )
    docker_run
    ;;
  build )
    docker_build
    ;;
  clear )
    docker_clear $*
    ;;
  help | usage )
    help_info
    ;;
  *)
    help_info
    ;;
  esac
}

main "$@"
