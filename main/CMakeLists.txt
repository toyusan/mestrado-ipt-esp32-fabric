idf_component_register(SRCS main.c
                            wifi_app.c
                            https_app.c
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES device-cert.pem
                                      device-key.pem
                                      ca-cert.pem)