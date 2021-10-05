
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sqlite3.h>


void save_file(std::string fnm, std::vector<uint8_t> &d)
{
    size_t start = 0;
    size_t len = d.size();

    if(d.size() < 0x26) {
        printf("%s size too small %ld\n", fnm.c_str(), d.size());
        return;
    }
    if(d[0x24] == 0xff && d[0x25] == 0xd8) {
        start = 0x24;
        len = (d[0x23] << 24) + (d[0x22] << 16) + (d[0x21] << 8) + d[0x20];
        if(start + len > d.size()) {
            printf("invalid len %ld\n", len);
            len = d.size() - start;
        }
    }

    std::ofstream outfile;
    outfile.open(fnm, std::ios::out | std::ios::binary); 
    outfile.write((const char *)&d[start], len);
}


int main()
{
sqlite3 *db = nullptr;
sqlite3_stmt *statement = nullptr;
int result = 0;

    if(sqlite3_open("0.db3", &db) != SQLITE_OK) {
        printf("Open database failed\n");
        return 0;
    }

    const char* sql = "SELECT data FROM messages where topic_id = 7 order by timestamp";
    if(sqlite3_prepare_v2(db, sql, strlen(sql), &statement, 0) != SQLITE_OK) {
        printf("Open database failed\n");
        return 0;
    }

    for(int i=0; ; i++) {
        result = sqlite3_step(statement);
        if(result == SQLITE_ROW) {
            int size = sqlite3_column_bytes(statement, 0);

            std::vector<uint8_t> data;
            data.resize(size);
            memcpy(data.data(), sqlite3_column_blob(statement, 0), size);

            char fnm[100] = {0};
            snprintf(fnm, sizeof(fnm), "images/img_%04d.jpg", i);
            save_file(fnm, data);
        } else {
            break;
        }
    }
    sqlite3_finalize(statement);
    sqlite3_close(db);
    return 0;
}
