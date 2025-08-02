#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define INDENT_SIZE 4

// ANSI Colors
#define RESET        "\033[0m"
#define BOLD         "\033[1m"
#define FG_BLUE      "\033[34m"
#define FG_CYAN      "\033[36m"
#define FG_MAGENTA   "\033[35m"
#define FG_YELLOW    "\033[33m"
#define FG_GREEN     "\033[32m"
#define FG_WHITE     "\033[37m"
#define FG_GRAY      "\033[90m"
#define FG_RED       "\033[31m"

// Utility: Get file extension
const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

// Check if file is directory
int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) return 1;
    return 0;
}

// Grouping logic
const char* group_file(const char *filename) {
    const char *ext = get_extension(filename);

    if (strcmp(ext, "c") == 0 || strcmp(ext, "cpp") == 0 || strcmp(ext, "h") == 0 ||
        strcmp(ext, "js") == 0 || strcmp(ext, "ts") == 0 || strcmp(ext, "py") == 0 ||
        strcmp(ext, "java") == 0 || strcmp(ext, "rb") == 0 || strcmp(ext, "go") == 0 ||
        strcmp(ext, "rs") == 0 || strcmp(ext, "php") == 0 || strcmp(ext, "sh") == 0)
        return "Code";

    if (strcmp(ext, "mp3") == 0 || strcmp(ext, "wav") == 0 || strcmp(ext, "flac") == 0)
        return "Audio";

    if (strcmp(ext, "mp4") == 0 || strcmp(ext, "mkv") == 0 || strcmp(ext, "mov") == 0)
        return "Video";

    if (strcmp(ext, "pdf") == 0 || strcmp(ext, "doc") == 0 || strcmp(ext, "docx") == 0 || strcmp(ext, "txt") == 0)
        return "Document";

    if (strcmp(ext, "png") == 0 || strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0 || strcmp(ext, "gif") == 0)
        return "Image";

    return "Other";
}

const char* get_color(const char *group) {
    if (strcmp(group, "Code") == 0) return FG_BLUE;
    if (strcmp(group, "Audio") == 0) return FG_CYAN;
    if (strcmp(group, "Video") == 0) return FG_MAGENTA;
    if (strcmp(group, "Document") == 0) return FG_YELLOW;
    if (strcmp(group, "Image") == 0) return FG_GREEN;
    return FG_GRAY;
}

void print_with_timestamp(const char *path, const char *filename) {
    struct stat st;
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, filename);
    if (stat(full_path, &st) == 0) {
        char timebuf[64];
        struct tm *tm_info = localtime(&st.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("%-30s " FG_GRAY "Modified: %s\n" RESET, filename, timebuf);
    } else {
        perror("stat");
    }
}

void print_tree(const char *path, int depth) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        for (int i = 0; i < depth * INDENT_SIZE; ++i)
            printf(" ");
        printf(FG_WHITE "|-- %s\n" RESET, entry->d_name);

        char new_path[512];
        snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
        if (is_directory(new_path)) {
            print_tree(new_path, depth + 1);
        }
    }
    closedir(dir);
}

void list_grouped(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct { char name[256]; char group[50]; } files[1024];
    int count = 0;

    while ((entry = readdir(dir)) != NULL && count < 1024) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(files[count].name, sizeof(files[count].name), "%s", entry->d_name);
        snprintf(files[count].group, sizeof(files[count].group), "%s", group_file(entry->d_name));
        count++;
    }
    closedir(dir);

    const char *groups[] = {"Code", "Audio", "Video", "Document", "Image", "Other"};
    int group_count = sizeof(groups) / sizeof(groups[0]);

    for (int g = 0; g < group_count; ++g) {
        printf("\n%s%s FILES%s\n", BOLD FG_WHITE, groups[g], RESET);
        printf("%s====================================%s\n", FG_GRAY, RESET);
        for (int i = 0; i < count; ++i) {
            if (strcmp(files[i].group, groups[g]) == 0) {
                const char *color = get_color(files[i].group);
                printf("   %s- %s%s\n", color, files[i].name, RESET);
            }
        }
    }
}

int has_extension(const char *filename, const char *ext) {
    const char *file_ext = get_extension(filename);
    return strcmp(file_ext, ext) == 0;
}

void list_dir(const char *path, int show_hidden, int only_files, int only_dirs, const char *ext_filter) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.')
            continue;

        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        int is_dir = is_directory(full_path);
        if ((only_dirs && !is_dir) || (only_files && is_dir))
            continue;

        if (ext_filter && !has_extension(entry->d_name, ext_filter))
            continue;

        if (is_dir)
            printf(BOLD FG_BLUE "%s/\n" RESET, entry->d_name);
        else
            printf(BOLD FG_WHITE "%s\n" RESET, entry->d_name);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *path = ".";
    int show_hidden = 0, only_files = 0, only_dirs = 0;
    const char *ext_filter = NULL;

    if (argc < 2) {
        list_dir(path, 0, 0, 0, NULL);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-tr") == 0) {
            printf(BOLD FG_WHITE "\nDirectory Tree:\n" RESET);
            printf(FG_GRAY "------------------------------\n" RESET);
            print_tree(path, 0);
            return 0;
        } else if (strcmp(argv[i], "-t") == 0) {
            DIR *dir = opendir(path);
            if (!dir) { perror("opendir"); return 1; }
            struct dirent *entry;
            printf(BOLD FG_WHITE "\nFiles with Timestamps:\n" RESET);
            printf(FG_GRAY "------------------------------\n" RESET);
            while ((entry = readdir(dir)) != NULL)
                if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                    print_with_timestamp(path, entry->d_name);
            closedir(dir);
            return 0;
        } else if (strcmp(argv[i], "-g") == 0) {
            printf(BOLD FG_WHITE "\nGrouped File Listing:\n" RESET);
            printf(FG_GRAY "------------------------------\n" RESET);
            list_grouped(path);
            return 0;
        } else if (strcmp(argv[i], "-a") == 0) {
            show_hidden = 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            only_files = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            only_dirs = 1;
        } else if (strcmp(argv[i], "--ext") == 0 && i + 1 < argc) {
            ext_filter = argv[++i];
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            FILE *f = fopen(argv[++i], "r");
            if (!f) { printf(FG_RED "Cannot open file.\n" RESET); return 1; }
            char buf[1024];
            while (fgets(buf, sizeof(buf), f)) printf(FG_WHITE "%s" RESET, buf);
            fclose(f);
            return 0;
        } else if (strcmp(argv[i], "-h") == 0) {
            printf(BOLD "Usage:\n" RESET);
            printf("  peek             Show files & folders\n");
            printf("  peek -a          Include hidden files\n");
            printf("  peek -f          Show only files\n");
            printf("  peek -d          Show only directories\n");
            printf("  peek -tr         Show directory tree\n");
            printf("  peek -t          List files with timestamps\n");
            printf("  peek -g          Group files by type\n");
            printf("  peek -r <file>   Read file contents\n");
            printf("  peek --ext <ext> Filter by extension\n");
            return 0;
        } else {
            printf(FG_RED "Unknown flag: %s\n" RESET, argv[i]);
            return 1;
        }
    }

    list_dir(path, show_hidden, only_files, only_dirs, ext_filter);
    return 0;
}
