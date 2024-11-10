#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// mpic++ -std=c++17 task1.cpp -o build/MPI_Servers_and_files/MPI_Servers_and_files_Task1
// mpirun --map-by :oversubscribe -n 2 build/MPI_Servers_and_files/MPI_Servers_and_files_Task1

int main(int argc, char *argv[]) {
  try {
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Server part
    if (rank == 0) {
      MPI_Comm client;
      MPI_Status status;
      char portname[MPI_MAX_PORT_NAME];

      MPI_Open_port(MPI_INFO_NULL, portname);
      std::cout << "[server] opened server with portname: " << portname
                << std::endl;
      MPI_Info info;
      MPI_Info_create(&info);
      MPI_Info_set(info, "ompi_local_scope", "true");
      MPI_Publish_name("my_server", info, portname);

      if (MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_SELF, &client) !=
          MPI_SUCCESS) {
        std::cout << "[server] could not accept a client" << std::endl;
      }

      std::cout << "[server] client connected" << std::endl;

      char message[32];
      MPI_Recv(message, 32, MPI_CHAR, MPI_ANY_SOURCE, 0, client, &status);
      std::cout << "[server] received message = \"" << message << "\""
                << std::endl;

      sleep(1);
      MPI_Unpublish_name("my_server", MPI_INFO_NULL, portname);

      MPI_Comm_free(&client);
      MPI_Close_port(portname);
      std::cout << "[server] closed" << std::endl;
    }
    // Client part
    if (rank == 1) {
      // Imitate delay as for real server-client
      sleep(1);
      MPI_Comm server;
      char portname[MPI_MAX_PORT_NAME];
      if (MPI_Lookup_name("my_server", MPI_INFO_NULL, portname) !=
          MPI_SUCCESS) {
        std::cout << "[client] could not find server" << std::endl;
        return 1;
      }
      std::cout << "[client] found server with portname: " << portname
                << std::endl;

      char message[32] = "Hello world!";
      std::cout << "[client] sending message to server \"" << message << "\""
                << std::endl;
      if (MPI_Comm_connect(portname, MPI_INFO_NULL, 0, MPI_COMM_SELF,
                           &server) != MPI_SUCCESS) {
        std::cout << "[client] could not connect to the server" << std::endl;
        return 1;
      }
      if (MPI_Send(message, 32, MPI_CHAR, 0, 0, server) != MPI_SUCCESS) {
        std::cout << "[client] could not send data to the server" << std::endl;
      }

      if (MPI_Comm_free(&server) != MPI_SUCCESS) {
        std::cout << "[client] could not disconnect from server" << std::endl;
      } else {
        std::cout << "[client] disconnected from server" << std::endl;
      }
    }
    MPI_Finalize();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
