/*
 * Copyright (C) 2015 Kevin Chan (kevin@gembler.org)
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <unistd.h>

typedef struct {
    ngx_array_t *args;
} ngx_http_print_conf_t;

static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;

static ngx_int_t ngx_http_print_filter(ngx_http_request_t *r);

static ngx_int_t ngx_http_print_filter_init(ngx_conf_t *cf);

static void *ngx_http_print_create_conf(ngx_conf_t *cf);

static char *ngx_http_print_merge_conf(ngx_conf_t *cf, void *parent, 
    void *child);

static ngx_command_t ngx_http_print_commands[] = {
    {
        ngx_string("print_hostname"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                          |NGX_CONF_TAKE1,
        ngx_conf_set_str_array_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_print_conf_t, args),
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_print_module_ctx = {
    NULL,                                   /* preconfiguration */
    ngx_http_print_filter_init,             /* postconfiguration */

    NULL,                                   /* create main configuration */
    NULL,                                   /* init main configuration */

    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */

    ngx_http_print_create_conf,             /* create location configuration */
    ngx_http_print_merge_conf               /* merge location configuration */
};

ngx_module_t ngx_http_print_module = {
    NGX_MODULE_V1,
    &ngx_http_print_module_ctx,             /* module context */
    ngx_http_print_commands,                /* module directives */
    NGX_HTTP_MODULE,                        /* module type */
    NULL,                                   /* init master */
    NULL,                                   /* init module */
    NULL,                                   /* init process */
    NULL,                                   /* init thread */
    NULL,                                   /* exit thread */
    NULL,                                   /* exit process */
    NULL,                                   /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_print_create_conf(ngx_conf_t *cf)
{
    ngx_http_print_conf_t *conf;
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_print_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    conf->args = NGX_CONF_UNSET_PTR;
    return conf;
}

static char *ngx_http_print_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_print_conf_t *prev = parent;
    ngx_http_print_conf_t *conf = child;
    if (conf->args == NULL) {
        conf->args = prev->args;
    }

    return NGX_CONF_OK;
}

static ngx_int_t 
ngx_http_print_filter(ngx_http_request_t *r)
{
    ngx_table_elt_t *h;
    ngx_http_print_conf_t *conf;
    ngx_log_t *log;

    log = r->connection->log;
    conf = ngx_http_get_module_loc_conf(r, ngx_http_print_module);

    if (conf->args)
    {
        ngx_str_t *args = conf->args->elts;

        char hostname[1024];
        hostname[1023] = '\0';
        gethostname(hostname, 1023);

        ngx_str_t ngx_str_hostname;
        ngx_str_hostname.len = strlen(hostname);
        ngx_str_hostname.data = (u_char *) hostname;

        for (ngx_uint_t i = 0; i < conf->args->nelts; i++)
        {
            h = ngx_list_push(&r->headers_out.headers);
            h->hash = 1;
            h->key = args[i];
            h->value = ngx_str_hostname;
        }
    }

    return ngx_http_next_header_filter(r);
}

static ngx_int_t 
ngx_http_print_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_print_filter;

    return NGX_OK;
}
