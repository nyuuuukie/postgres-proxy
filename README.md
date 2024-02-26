# postgres-proxy

This project is a simple proxy server that allows to log all sql requests sending through it.

## Compilation
```bash
# Compiles server
make

# Re-compiles server and cleanes logs directory
make re
```

## Run

### Flags

| Short flag | Long flag | Description | Default value | 
| ------------- | ------------- | ------------- | ------------- |
| -th | --target-host  | Target host (IPv4 addr) | 127.0.0.1 |
| -tp | --target-port  | Target port (IPv4 addr) | 5432 |
| -h | --host  | Proxy host (current machine) | 127.0.0.1 |
| -p | --port  | Proxy port (current machine) | 8080 |
| -w | --workers  | Workers(threads) count | 3 |
| -d | --log-dir  | Logs directory | logs |
| -l | --log-lvl  | Logs level | 2 (<span style="color : lawngreen">INFO</span>) |
| -a | --log-all-msg | Log all messages | false |

### Log levels
| Level | Prefix | Description | 
| ------------- | ------------- | ------------- |
| 0 | <span style="color : crimson">CRIT</span>  | Critical errors logs only (system functions failure) |
| 1 | <span style="color : firebrick">ERROR</span>  | All error logs |
| 2 | <span style="color : lawngreen">INFO</span>  | Errors and information logs |
| 3 | <span style="color : khaki">DEBUG</span> | Errors, info and debug logs |


### Examples:

```bash
# Use default
./pg_proxy 

# Tries to connect to db on 10.11.12.13:6789
pg_proxy -th "10.11.12.13" -tp 6789

# Launches proxy server on 4444 port
pg_proxy --port 4444

# Log level set to DEBUG level
pg_proxy -l 3 

# Directory with logs
pg_proxy -d <logdir>

# Worker threads count set to 4
./pg_proxy -w 4
```

## Description

This server uses two loggers of Logger class:
- The first one (Log) used to output server's log messages.
- The second used to output parsed query messages.

Loggers set up separately. </br>
By default the first outputs to stdout and to file,
and the second outputs only to the file by default (prefixed with "query-").

### Logs examples


Query logs:
```log
2024-02-26 21:46:04  INFO Logging into logs/query-26-02-2024_21-46-04.log
2024-02-26 21:46:14 SHOW config_file;
2024-02-26 21:46:38 select * from information_schema.tables limit 1;
2024-02-26 22:25:43 select * from information_schema.tables limit 50;
2024-02-26 22:25:55 select * from information_schema.tables limit 60;
2024-02-26 22:26:00 select * from information_schema.tables limit 70;
2024-02-26 22:26:04 select * from information_schema.tables limit 80;
2024-02-26 22:26:06 select * from information_schema.tables limit 90;
2024-02-26 22:26:10 select * from information_schema.tables limit 95;
2024-02-26 22:26:12 select * from information_schema.tables limit 99;
2024-02-26 22:26:17 select * from information_schema.tables limit 100;
2024-02-26 22:26:21 select * from information_schema.tables limit 110;
2024-02-26 22:26:24 select * from information_schema.tables limit 120;
2024-02-26 22:26:40 select * from information_schema.tables limit 130;
2024-02-26 22:26:43 select * from information_schema.tables limit 140;
2024-02-26 22:26:46 select * from information_schema.tables limit 150;
```

Parsed messages:
```log
2024-02-26 20:31:08  INFO Logging into logs/query-26-02-2024_20-31-08.log
2024-02-26 20:31:12     8 [4][210][22]/
2024-02-26 20:31:12 N   4 
2024-02-26 20:31:12    84 [0][3][0][0]user[0]mhufflep[0]database[0]postgres[0]application_name[0]psql[0]client_encoding[0]UTF8[0][0]
2024-02-26 20:31:12 R   8 [0][0][0][0]
2024-02-26 20:31:12 S  26 application_name[0]psql[0]
2024-02-26 20:31:12 S  25 client_encoding[0]UTF8[0]
2024-02-26 20:31:12 S  23 DateStyle[0]ISO, MDY[0]
2024-02-26 20:31:12 S  38 default_transaction_read_only[0]off[0]
2024-02-26 20:31:12 S  23 in_hot_standby[0]off[0]
2024-02-26 20:31:12 S  25 integer_datetimes[0]on[0]
2024-02-26 20:31:12 S  27 IntervalStyle[0]postgres[0]
2024-02-26 20:31:12 S  20 is_superuser[0]on[0]
2024-02-26 20:31:12 S  25 server_encoding[0]UTF8[0]
2024-02-26 20:31:12 S  36 server_version[0]14.11 (Homebrew)[0]
2024-02-26 20:31:12 S  35 session_authorization[0]mhufflep[0]
2024-02-26 20:31:12 S  35 standard_conforming_strings[0]on[0]
2024-02-26 20:31:12 S  32 TimeZone[0]Asia/Yekaterinburg[0]
2024-02-26 20:31:12 K  12 [0][0][20]}[190][197][188]e
2024-02-26 20:31:12 Z   5 I
2024-02-26 20:31:14 Q  45 select * from information_schema.tables;[0]
```

Server logs:
```log
2024-02-26 20:12:57  INFO Logging into logs/26-02-2024_20-12-57.log
2024-02-26 20:12:57  INFO Logging into logs/query-26-02-2024_20-12-57.log
2024-02-26 20:12:57  INFO Listening on 127.0.0.1:8080
2024-02-26 20:12:57  INFO Server is starting on 127.0.0.1:8080
2024-02-26 20:12:57  INFO Proxying to 127.0.0.1:5432
2024-02-26 20:12:57  INFO Log level: 3
2024-02-26 20:12:57  INFO Log directory: logs
2024-02-26 20:12:57 DEBUG Worker 0x16b7d3000: cycle started
2024-02-26 20:12:57 DEBUG Worker 0x16b85f000: cycle started
2024-02-26 20:12:57 DEBUG Worker 0x16b8eb000: cycle started
2024-02-26 20:13:02 DEBUG Server::connect [6] -> [7]
2024-02-26 20:13:02 DEBUG Client::addReadEvent [6]: READ_REQUEST event added
2024-02-26 20:13:02 DEBUG Client::addReadEvent [6]: READ_REQUEST event added
2024-02-26 20:13:02 DEBUG Client::addParseEvent [6]: PARSE_REQUEST event added
2024-02-26 20:13:02 DEBUG Client::addParseEvent [6]: PARSE_REQUEST event added
2024-02-26 20:13:02 DEBUG Client::addPassEvent [7]: PASS_REQUEST event added
2024-02-26 20:13:02 DEBUG Socket::write [7]: 8/8 bytes
2024-02-26 20:13:02 DEBUG Client::addReadEvent [7]: READ_RESPONSE event added
2024-02-26 20:13:02 DEBUG Client::addParseEvent [7]: PARSE_RESPONSE event added
2024-02-26 20:13:02 DEBUG SSL response
2024-02-26 20:13:02 DEBUG Client::addParseEvent [7]: PARSE_RESPONSE event added
2024-02-26 20:13:02 DEBUG Client::addPassEvent [6]: PASS_RESPONSE event added
2024-02-26 20:13:02 DEBUG Socket::write [6]: 1/1 bytes
```
