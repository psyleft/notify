#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libguile.h>
#include <curl/curl.h>
#include <config.h>

static void
inner_main (void *closure, int argc, char **argv)
{
    (void) closure;

    size_t arg = 0;
    char *program_name = argv[arg++];

    if (argc != 2) {
        fprintf(stderr, "[ERROR] Incorrect number of arguments\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s: [notification]\n", program_name);

        exit(1);
    }

    char *notification = argv[arg++];

    SCM guile_dir = scm_from_latin1_string(GUILE_DIR);
    SCM guile_script_name = scm_from_latin1_string(notification);
    SCM guile_extension = scm_from_latin1_string(".scm");

    SCM guile_script_path =
        scm_string_join(
            scm_list_3(guile_dir, guile_script_name, guile_extension),
            scm_string(SCM_EOL),
            scm_from_latin1_symbol("infix")
        );

    bool access = scm_is_true(scm_access(guile_script_path,
                                         scm_from_int(4))); // R_OK
    if (!access) {
        fprintf(stderr, "[ERROR] Cannot access file %s\n",
                scm_to_latin1_stringn(guile_script_path, NULL));
        exit(1);
    }

    SCM json_str = scm_primitive_load(guile_script_path);
    char *json_cstr = scm_to_latin1_stringn(json_str, NULL);

    CURL *handle;
    CURLcode result;

    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();

    if (!handle) {
        fprintf(stderr, "[ERROR] Curl init failed\n");
        exit(1);
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: text/json");

#ifdef NTFY_AUTH
    headers = curl_slist_append(headers, "Authorization: Basic " NTFY_AUTH);
#endif // NTFY_AUTH

#ifdef NTFY_TOKEN
    headers = curl_slist_append(headers, "Authorization: Bearer " NTFY_TOKEN);
#endif // NTFY_TOKEN

    curl_easy_setopt(handle, CURLOPT_URL, NTFY_SERVER_URL);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_cstr);
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    result = curl_easy_perform(handle);

    if (result != CURLE_OK) {
        fprintf(stderr, "[ERROR] Curl preform failed: %s\n",
                curl_easy_strerror(result));
        exit(1);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(handle);
    curl_global_cleanup();

    exit(0);
}

int
main(int argc, char **argv)
{
    scm_boot_guile(argc, argv, inner_main, 0);
    return -1; // Unreachable
}
