/*
 * Copyright © 2011 Donghee PARK <dongheepark@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <modbus.h>
#include "arduino-serial.h"

#define SERVER_ID        1 
#define P0_ADDRESS      0


void x_axis_turn_right( modbus_t * ctx)
{
    int rc;
    rc = modbus_write_register(ctx, P0_ADDRESS,  0x0110); // v 
    
    if (rc != 1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        printf("ERROR X 연속 드라이브 설정 (%d)\n", rc);
        printf("Address = %d, value = %d (0x%X)\n",
               P0_ADDRESS, 0x0110, 0x0110);
    }
}

void x_axis_turn_left( modbus_t * ctx)
{
    int rc;
    rc = modbus_write_register(ctx, P0_ADDRESS,  0x0120); // ^
    
    if (rc != 1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        printf("ERROR X 연속 드라이브 설정 (%d)\n", rc);
        printf("Address = %d, value = %d (0x%X)\n",
               P0_ADDRESS, 0x0110, 0x0110);
    }
}

void x_axis_set_speed(modbus_t * ctx, int speed)
{
    int rc;
    rc = modbus_write_register(ctx, P0_ADDRESS,  0x0120); // ^
}

void x_axis_stop(modbus_t * ctx)
{
        int rc;

        rc = modbus_write_register(ctx, P0_ADDRESS,  0x0501); //05: 감속 정지 01: x축 02: y축
    
        if (rc != 1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            printf("ERROR 감속 정지 설정 (%d)\n", rc);
            printf("Address = %d, value = %d (0x%X)\n",
                   P0_ADDRESS, 0x0110, 0x0110);
        }
        usleep(300000);
}


int main(void)
{
    modbus_t *ctx;
    int i;
    uint16_t tab_reg[64];

    int rc;
    uint16_t b[3] = {0x6101, 0x0fa0, 0x0fa0};
    
    int fd =0;
    char buf[256];
    char value;

    ctx = modbus_new_rtu("/dev/ttyUSB0", 38400, 'N', 8, 1);
    modbus_set_slave(ctx, SERVER_ID); // 엄지 선택
	modbus_set_debug(ctx, 1);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    usleep(15000);
    x_axis_stop(ctx);

    // Arduino init
    fd = serialport_init("/dev/ttyUSB1", 9600);
    
    while (1)
        {
            value = serialport_read(fd);
            if (value != -1) {
                printf("From arduino: %d \n",value);
            }
            usleep(1500);

            switch (value) {
            case 1:
                printf("엄지 선택\n");
                modbus_set_slave(ctx, 1);
                break;
            case 2:
                printf("검지 선택 \n");
                modbus_set_slave(ctx, 2);
                break;
            case 3:
                printf("중지 선택\n");
                modbus_set_slave(ctx, 3);
                break;

            case 4:
                printf("약지 선택\n");
                modbus_set_slave(ctx, 4);
                break;

            case 5:
                printf("약지 선택\n");
                modbus_set_slave(ctx, 5);
                break;
                
            case 'b' :
                printf("b\n");
                x_axis_stop(ctx);
                x_axis_turn_right(ctx);
                break;
            case 'f' :
                printf("f\n");
                x_axis_stop(ctx);
                x_axis_turn_left(ctx);            
                break;                
            }
        }
    
    x_axis_stop(ctx);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
