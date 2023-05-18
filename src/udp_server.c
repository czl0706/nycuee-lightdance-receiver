#include "udp_server.h"

static const char *TAG = "UDP_Server";

// static void udp_server_task(void *pvParameters)
// {
//     char rx_buffer[128];
//     char addr_str[128];
//     int addr_family = AF_INET;
//     int ip_protocol = 0;
//     struct sockaddr_in6 dest_addr;
//     void (*datarecv_cb)(uint8_t *, int) = (void *)pvParameters; 

//     while (1) {

//         // if (addr_family == AF_INET) {
//             struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
//             dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
//             dest_addr_ip4->sin_family = AF_INET;
//             dest_addr_ip4->sin_port = htons(PORT);
//             ip_protocol = IPPROTO_IP;
//         // } else if (addr_family == AF_INET6) {
//         //     bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
//         //     dest_addr.sin6_family = AF_INET6;
//         //     dest_addr.sin6_port = htons(PORT);
//         //     ip_protocol = IPPROTO_IPV6;
//         // }

//         int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
//         if (sock < 0) {
//             ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
//             break;
//         }
//         ESP_LOGI(TAG, "Socket created");

// #if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
//         int enable = 1;
//         lwip_setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &enable, sizeof(enable));
// #endif
//         // Set timeout
//         struct timeval timeout = {
//             .tv_sec = 30,
//             .tv_usec = 0
//         };
//         setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

//         int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//         if (err < 0) {
//             ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
//         }
//         ESP_LOGI(TAG, "Socket bound, port %d", PORT);

//         struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
//         socklen_t socklen = sizeof(source_addr);

// #if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
//         struct iovec iov;
//         struct msghdr msg;
//         struct cmsghdr *cmsgtmp;
//         u8_t cmsg_buf[CMSG_SPACE(sizeof(struct in_pktinfo))];

//         iov.iov_base = rx_buffer;
//         iov.iov_len = sizeof(rx_buffer);
//         msg.msg_control = cmsg_buf;
//         msg.msg_controllen = sizeof(cmsg_buf);
//         msg.msg_flags = 0;
//         msg.msg_iov = &iov;
//         msg.msg_iovlen = 1;
//         msg.msg_name = (struct sockaddr *)&source_addr;
//         msg.msg_namelen = socklen;
// #endif

//         while (1) {
//             ESP_LOGI(TAG, "Waiting for data");
// #if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
//             int len = recvmsg(sock, &msg, 0);
// #else
//             int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
// #endif
//             // Error occurred during receiving
//             if (len < 0) {
//                 ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
//                 break;
//             }
//             // Data received
//             else {
//                 // Get the sender's ip address as string
//                 // if (source_addr.ss_family == PF_INET) {
//                     inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
// #if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
//                     for ( cmsgtmp = CMSG_FIRSTHDR(&msg); cmsgtmp != NULL; cmsgtmp = CMSG_NXTHDR(&msg, cmsgtmp) ) {
//                         if ( cmsgtmp->cmsg_level == IPPROTO_IP && cmsgtmp->cmsg_type == IP_PKTINFO ) {
//                             struct in_pktinfo *pktinfo;
//                             pktinfo = (struct in_pktinfo*)CMSG_DATA(cmsgtmp);
//                             ESP_LOGI(TAG, "dest ip: %s\n", inet_ntoa(pktinfo->ipi_addr));
//                         }
//                     }
// #endif
//                 // } else if (source_addr.ss_family == PF_INET6) {
//                 //     inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
//                 // }

//                 rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
//                 if (datarecv_cb) {
//                     datarecv_cb((uint8_t *)rx_buffer, len);
//                 }
//                 ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
//                 ESP_LOGI(TAG, "%s", rx_buffer);

//                 // int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
//                 // if (err < 0) {
//                 //     ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
//                 //     break;
//                 // }
//             }
//         }

//         if (sock != -1) {
//             ESP_LOGE(TAG, "Shutting down socket and restarting...");
//             shutdown(sock, 0);
//             close(sock);
//         }
//     }
//     vTaskDelete(NULL);
// }

static void udp_server_task(void *pvParameters)
{
    // char rx_buffer[128];
    // char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;
    buffer_t* buffer = pvParameters; 

    while (1) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        // Set timeout
        // struct timeval timeout = {
        //     .tv_sec = 30,
        //     .tv_usec = 0
        // };
        
        // setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            shutdown(sock, 0);
            close(sock);
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            continue;
        }
        // ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

        while (1) {
            // ESP_LOGI(TAG, "Waiting for data");
            int len = recvfrom(sock, buffer->data, BUFFER_SIZE - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                // break;
            } else {
                buffer->available = true;
                buffer->data[len] = 0; // Null-terminate whatever we received and treat like a string...

                // ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                // ESP_LOGI(TAG, "%s", buffer->data);
            }

            // // ??
            // if (sock != -1) {
            //     ESP_LOGE(TAG, "Shutting down socket and restarting...");
            //     shutdown(sock, 0);
            //     close(sock);
            // }
        }
    }
    vTaskDelete(NULL);
}

void udp_server_init(buffer_t* buffer) 
{
    xTaskCreate(udp_server_task, "udp_server", 4096, (void *)buffer, 5, NULL);    
}