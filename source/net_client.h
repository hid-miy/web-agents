/**
 * The contents of this file are subject to the terms of the Common Development and
 * Distribution License (the License). You may not use this file except in compliance with the
 * License.
 *
 * You can obtain a copy of the License at legal/CDDLv1.0.txt. See the License for the
 * specific language governing permission and limitations under the License.
 *
 * When distributing Covered Software, include this CDDL Header Notice in each file and include
 * the License file at legal/CDDLv1.0.txt. If applicable, add the following below the CDDL
 * Header, with the fields enclosed by brackets [] replaced by your own identifying
 * information: "Portions copyright [year] [name of copyright owner]".
 *
 * Copyright 2014 - 2016 ForgeRock AS.
 */

#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "http_parser.h"
#include "thread.h"

typedef struct {
    int cert_key_pass_sz;
    int proxy_password_sz;
    int local;
    int lb_enable;
    int net_timeout;
    int keepalive;
    int cert_trust;
    int hostmap_sz;
    int notif_enable;
    int secure_channel_disable;
    int proxy_port;
    char *proxy_host;
    char *proxy_user;
    char *proxy_password;
    char *notif_url;
    char *server_id;
    char *ciphers;
    char *cert_ca_file;
    char *cert_file;
    char *cert_key_file;
    char *cert_key_pass;
    char *tls_opts;
    char **hostmap;
    void (*log)(const char *, ...);
} am_net_options_t;

typedef struct {
#ifdef _WIN32
    SOCKET sock;
#else
    int sock;
#endif
    unsigned long instance_id;

    unsigned int retry;
    unsigned int retry_wait; /* in seconds */

    const char *url;
    struct url uv;
    char *req_headers;
    char **header_fields;
    char **header_values;
    int req_method;

    struct ssl {
        char on;
        void *ssl_handle;
        void *ssl_context;
        void *read_bio;
        void *write_bio;
        int error;
        int sys_error;
        char *request_data;
        size_t request_data_sz;
    } ssl;

    am_net_options_t *options;

    http_parser_settings *hs;
    http_parser *hp;
    int header_state;
    int num_headers;
    int num_header_values;
    unsigned int http_status;

    enum {
        AM_PROXY_NONE = 0,
        AM_PROXY_CONNECTING,
        AM_PROXY_CONNECTED
    } proxy;

    struct addrinfo *ra;

    void *data;
    void (*on_connected)(void *udata, int status);
    void (*on_data)(void *udata, const char *data, size_t data_sz, int status);
    void (*on_complete)(void *udata, int status); /* callback when all data for the current request is read */
    void (*on_close)(void *udata, int status);

    void (*reset_complete)(void *udata);
    am_bool_t(*is_complete)(void *udata);
    int error;
} am_net_t;


int am_net_sync_connect(am_net_t *n);
int am_net_write(am_net_t *n, const char *data, size_t data_sz);
void am_net_sync_recv(am_net_t *n, int timeout_ms);
int am_net_close(am_net_t *n);

void am_net_options_create(am_config_t *ac, am_net_options_t *options, void (*log)(const char *, ...));
void am_net_options_delete(am_net_options_t *options);

int am_agent_login(unsigned long instance_id, const char *openam,
        const char *user, const char *pass, const char *realm, const char *eval_app, am_net_options_t *options,
        char **agent_token, char **pxml, size_t *pxsz, struct am_namevalue **session_list);
int am_agent_logout(unsigned long instance_id, const char *openam,
        const char *token, am_net_options_t *options);
int am_agent_policy_request(unsigned long instance_id, const char *openam,
        const char *token, const char *user_token, const char *req_url,
        const char *scope, const char *cip, const char *pattr, const char *eval_app,
        am_net_options_t *options, struct am_namevalue **session_list, struct am_policy_result **policy_list);
int am_url_validate(unsigned long instance_id, const char *url,
        am_net_options_t *options, int *httpcode);
int am_agent_audit_request(unsigned long instance_id, const char *openam,
        const char *logdata, am_net_options_t *options);

void am_net_init();
void am_net_shutdown();

#endif
