#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

static char buf[10240];

int main() {
  WSADATA wsa = {0};
  if (0 != WSAStartup(MAKEWORD(2, 2), &wsa)) return 1;

  struct addrinfo hints = {
    .ai_family   = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM,
    .ai_protocol = IPPROTO_TCP,
  };
  struct addrinfo * addr = NULL;
  if (0 != getaddrinfo("google.com", "80", &hints, &addr)) {
    WSACleanup();
    return 2;
  }

  SOCKET skt = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (skt == INVALID_SOCKET) {
    freeaddrinfo(addr);
    WSACleanup();
    return 3;
  }
  if (SOCKET_ERROR == connect(skt, addr->ai_addr, addr->ai_addrlen)) {
    closesocket(skt);
    WSACleanup();
    return 4;
  }

  freeaddrinfo(addr);

  const char * msg = "GET / HTTP/1.0\n\n";
  if (SOCKET_ERROR == send(skt, msg, strlen(msg), 0)) {
    closesocket(skt);
    WSACleanup();
    return 5;
  }

  if (SOCKET_ERROR == shutdown(skt, SD_SEND)) {
    closesocket(skt);
    WSACleanup();
    return 6;
  }

  int res;
  do {
    res = recv(skt, buf, sizeof(buf), 0);
    if (res > 0) fwrite(buf, 1, res, stdout);
  } while (res > 0);

  closesocket(skt);
  WSACleanup();
  return res == 0 ? 0 : 1;
}
