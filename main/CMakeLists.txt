idf_component_register(SRCS main.c
                            main_test.c
                            api/wifi_app.c
                            api/https_app.c
                            api/fw_update.c
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES cert/device-cert.pem
                                      cert/device-key.pem
                                      cert/ca-cert.pem)