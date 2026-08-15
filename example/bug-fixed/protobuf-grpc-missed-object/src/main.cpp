#include "service.grpc.pb.h"
#include <grpcpp/create_channel.h>

int main() {
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = regression::RegressionService::NewStub(channel);
    return stub ? 0 : 1;
}
