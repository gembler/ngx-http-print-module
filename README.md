# Nginx HTTP Print Module

The way to Print everything on Nginx.

## Installing

    ./configure --add-module=/path/to/ngx-http-print-module
    make
    make install

## Directives

### print_hostname


- Syntax:   **print_hostname** *header_name*;
- Default:  —
- Context:  http, server, location, if in location

Print **hostname** into Reponse Header.

## Example Configuration

    server {
        listen       8000;
        server_name  localhost;

        location / {
            print_hostname "X-Host";

            root   html;
            index  index.html;
        }
    }