#!/usr/bin/python3

import argparse
import logging
from operator import truediv
import subprocess
import os
from colorama import Fore, Style
from typing import Optional
import shutil
from pathlib import Path

logger = logging.getLogger("docker")

USER_NAME = os.environ.get('USER') or os.environ.get('USERNAME')
HOME = os.environ.get('HOME')
PROJECT_BASE_DIR = os.getcwd()


def is_protect_path(path):
    if not path:
        return False
    abspath = os.path.abspath(path)
    abspath = abspath.rstrip("/*")
    if abspath in set({"/", "/home", f"/home/{USER_NAME}"}):
        return True
    if os.path.isdir(abspath) and os.path.dirname(abspath) == "/":
        return True
    return False


def remove_file(path):
    if not path:
        return False
    try:
        if os.path.exists(path):
            if is_protect_path(path):
                logger.error(f"refused to remove the protected [{path}]")
                return False
            if os.path.islink(path):
                os.unlink(path)
            elif os.path.isdir(path):
                shutil.rmtree(path)
            else:
                os.remove(path)
        logger.info(f"remove file/dir/link [{path}]")
        return True
    except EnvironmentError as e:
        logger.error(e)
    logger.error(f"remove file [{path}] failed.")
    return False


def make_directory(path):
    if not path:
        return False
    try:
        if os.path.exists(path):
            if not os.path.isdir(path):
                remove_file(path)
                os.mkdir(path)
        else:
            os.makedirs(path)
        return True
    except EnvironmentError as e:
        logger.error(e)
    logger.error(f"make directory [{path}] failed.")
    return False


def init_logger():
    class ColorFormatter(logging.Formatter):
        COLORS = {
            "WARNING": Fore.YELLOW,
            "ERROR": Fore.RED,
            "CRITICAL": Fore.RED + Style.BRIGHT,
            "INFO": Fore.GREEN,
        }

        def format(self, record):
            color = self.COLORS.get(record.levelname, "")
            message = super().format(record)
            return f"{color}{message}{Style.RESET_ALL}"

    logger.setLevel(logging.DEBUG)
    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.INFO)
    console_handler.setFormatter(ColorFormatter('[%(asctime)s][%(levelname)s][%(filename)s:%(lineno)d] %(message)s'))
    logger.addHandler(console_handler)


def execute_shell_command(cmd: str, check: bool = True) -> Optional[str]:
    # check: 如果命令非零退出码则失败, 抛出异常
    # text: 以字符串返回输出, 默认返回字节流 bytes
    try:
        result = subprocess.run(cmd, shell=True, check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    except subprocess.CalledProcessError:
        logger.error(f"run cmd [{cmd}] failed")
    return result.stdout.strip() if result.stdout else None


def execute_shell_command_with_stdout(cmd: str):
    try:
        with subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True) as process:
            while True:
                output = process.stdout.readline()
                if output == '' and process.poll() is not None:
                    break
                if output:
                    print(output.strip())
            returncode = process.wait()
            if returncode == 0:
                logger.info(f"execute command [{cmd}] successfully")
                return True
            else:
                logger.warning(f"execute command [{cmd}] fail with ret code [{returncode}]")
                return False
    except subprocess.subprocess.CalledProcessError as e:
        logger.error(f"try execute shell command [{cmd}] fail with ret code [{returncode}]")
        return False


def if_docker_image_exist(
    docker_image: str,
) -> bool:
    images = execute_shell_command("docker images --format '{{.Repository}}:{{.Tag}}'")
    for line in images.splitlines():
        if line.strip() == docker_image:
            return True
    return False


def if_docker_container_exist(
    container_name: str,
) -> bool:
    cmd = "docker ps -a"
    containers = execute_shell_command(cmd + " --format '{{.Names}}'")
    for line in containers.splitlines():
        if line.strip() == container_name.strip():
            return True
    return False


def docker_build(
    docker_image: str,
    docker_container: str,
    docker_file: str,
) -> None:
    if if_docker_image_exist(docker_image):
        logger.info(f"docker image [{docker_image}] already exists")
    else:
        logger.warning(f"docker image [{docker_image}] don't exist")
        # 编译镜像
        docker_build_cmd = (
            ["docker", "build"]
            + [
                "--build-arg",
                f"USER_NAME={USER_NAME}",
                "--progress=plain",
                "-t",
                f"{docker_image}",
                "-f",
                f"{docker_file}",
                "."
            ]
        )
        execute_shell_command_with_stdout(' '.join(docker_build_cmd))

    if if_docker_container_exist(docker_container):
        logger.info(f"docker container [{docker_container}] already exists")
    else:
        gitconfig = Path.home() / ".gitconfig"
        if not gitconfig.is_file():
            logger.warning(f"touch [{gitconfig}]")
            gitconfig.touch()

        ssh_dir = Path.home() / ".ssh"
        if not ssh_dir.is_dir():
            logger.warning(f"mkdir [{ssh_dir}]")
            ssh_dir.mkdir(mode=0o700)

        # 创建容器
        # * --user $(id -u):$(id -g): 绑定宿主机 UID / GID
        # * -v /etc/passwd:/etc/passwd:ro: 共享用户信息
        docker_run_cmd = (
            ["docker", "run"]
            + [
                "-it", "-d",
                "--privileged",
                "--restart", "always",
                f"--name {docker_container}",
                "--env DOCKER_USER=root",
                f"--env USER={USER_NAME}",
                f"--env DOCKER_USER_ID={execute_shell_command('id -u')}",
                f"--env DOCKER_GRP={execute_shell_command('id -g -n')}",
                f"--env DOCKER_GRP_ID={execute_shell_command('id -g')}",
                f"--env DOCKER_IMG={docker_image}",
                f"--volume {PROJECT_BASE_DIR}:/{PROJECT_BASE_DIR}",
                f"--volume {HOME}/.gitconfig:{HOME}/.gitconfig",
                f"--volume {HOME}/.ssh:{HOME}/.ssh",
                f"--volume /etc/passwd:/etc/passwd:ro",
                f"--volume /etc/group:/etc/group:ro",
                f"--volume /etc/localtime:/etc/localtime:ro",
                "--volume /etc/resolv.conf:/etc/resolv.conf:ro",
                "--net host",
                "--add-host in_dev_docker:127.0.0.1",
                f"--add-host {execute_shell_command('hostname')}:127.0.0.1",
                "--hostname in_dev_docker",
                f"--workdir {PROJECT_BASE_DIR}",
            ] + [
                f"{docker_image}",
                "/bin/bash"
            ]
        )
        # print(' '.join(docker_run_cmd))
        execute_shell_command_with_stdout(' '.join(docker_run_cmd))


def docker_run(
    arch: str,
    docker_container: str,
) -> None:
    logger.info(f"start exec docker container [{docker_container}]")
    # 默认开发环境用当前用户
    # if arch == "ubuntu_2204":
    if True:
        docker_run_cmd = [
            "docker", "exec", "-it",
            docker_container,
            "/bin/bash", "-c",
            f"source /home/{USER_NAME}/.profile && /bin/bash"
        ]
    else:
        # 以 root 用户进入
        docker_run_cmd = [
            "docker", "exec", "-it",
            docker_container,
            "/bin/bash"
        ]

    docker_run_cmd_str = ' '.join(docker_run_cmd)
    logger.info(docker_run_cmd_str)

    # 直接运行命令, 保持交互式
    subprocess.run(docker_run_cmd, check=True)

def docker_clear(
    docker_image: str,
    docker_container: str,
) -> None:
    if if_docker_container_exist(docker_container):
        logger.info(f"stop docker container [{docker_container}]")
        stop_container_cmd = [
            "docker", "container", "stop", docker_container
        ]
        execute_shell_command_with_stdout(' '.join(stop_container_cmd))
        remove_container_cmd = [
            "docker", "container", "rm", "-f", docker_container
        ]
        execute_shell_command_with_stdout(' '.join(remove_container_cmd))
    else:
        logger.warning(f"docker container [{docker_container}] don't exist")
    
    if if_docker_image_exist(docker_image):
        logger.info(f"remove docker image [{docker_image}]")
        remove_image_cmd = [
            "docker", "rmi", docker_image
        ]
        execute_shell_command_with_stdout(' '.join(remove_image_cmd))
    else:
        logger.warning(f"docker image [{docker_image}] don't exist")
    

if __name__ == "__main__":
    init_logger()

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-A",
        "--arch",
        type=str,
        default="ubuntu_2204",
        choices=["ubuntu_2204", "ubuntu_latest"],
        help="architecture",
    )
    parser.add_argument(
        "-C",
        "--command",
        type=str,
        default="build",
        choices=["build", "run", "clear", "rebuild"],
        help="command",
    )
    args = parser.parse_args()
    logger.info(f"parsed args: {args}")

    docker_image = f"xmake-{args.arch}:latest"
    docker_container = f"xmake-dev-{args.arch}-container"
    docker_file = f"Dockerfile.{args.arch}"

    # match args.command:
    #     case "build":
    #         docker_build(docker_image, docker_container, docker_file)
    #     case "run":
    #         docker_run(args.arch, docker_container)
    #     case "clear":
    #         docker_clear(docker_image, docker_container)
    #     case "rebuild":
    #         docker_clear(docker_image, docker_container)
    #         docker_build(docker_image, docker_container, docker_file)
    #     case _:
    #         logger.error(f"invalid command [{args.command}]")
    #         exit(1)
    if args.command == "build":
        docker_build(docker_image, docker_container, docker_file)
    elif args.command == "run":
        docker_run(args.arch, docker_container)
    elif args.command == "clear":
        docker_clear(docker_image, docker_container)
    elif args.command == "rebuild":
        docker_clear(docker_image, docker_container)
        docker_build(docker_image, docker_container, docker_file)
    else:
        logger.error(f"invalid command [{args.command}]")
        exit(1)
