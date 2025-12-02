# SQLite-Database-Linux-Development
在此範例會去建立一個SQLite 的範例，並且移植到OrangePi 開發板上進行使用。首先去生成SQLite 可於OrangePi 執行的程式與函式庫後，透過所建立C語言應用程式，使用SQLite API 完成資料庫的SQL 語法。

## SQLite資料庫

SQLite 是一種「輕量級的資料庫」，它：
* 不需要伺服器（不像 MySQL、PostgreSQL）
* 資料全部存在一個 .db 檔案裡
* 適合嵌入式系統（像 firmware、Android app、桌面程式）

在接下來的範例會去建立一個SQLite Data Base的範例到ARM開發平台上

## SQLite安裝流程
### 1. 從官網下載sqlite exe檔，並將期進行解壓縮
```
tar zcvf ./sqliteEXE.tar.gz  ./exe
```
本範例使用的 SQLite 的版本為 sqlite-autoconf-3490100
![01](https://hackmd.io/_uploads/B1FkI2T1Wg.png)

[sqlite官網連結](https://sqlite.org/)

### 2. 裝指定的 cross-compiler
首先我們同樣使用虛擬機 (Virtual Box) 所建立的 Linux 系統，去執行所使用的ARM 版的 cross-compiler 安裝。

指令如下:

```javascript
sudo apt update
sudo apt install gcc-aarch64-linux-gnu 
```

### 3. 建立 configure 腳本

接下來來設定一個 cross-compilation（交叉編譯設定）的 configure 腳本，針對 ARM 架構（aarch64）來編譯程式，

```javascript
CC=aarch64-linux-gnu-gcc  AR=aarch64-linux-gnu-ar  ./configure --prefix=/home/joey/sqlite/exe --host=aarch64-linux-gnu
```

詳細的configure參數說明如下表所示:
| 參數                                             | 說明                             |
| ---------------------------------------------- | ------------------------------ |
| `CC=aarch64-linux-gnu-gcc`                     | 指定 cross compiler 的 C 編譯器      |
| `AR=aarch64-linux-gnu-ar`                      | 指定 cross compiler 的靜態庫打包工具     |
| `--host=aarch64-linux-gnu`                     | 指定要編譯給哪個平台（target platform）    |
| `--prefix=/home/joey/sqlite/exe`               | 安裝輸出目錄（`make install` 時會安裝到這裡） |


編譯 makefile 的執行過程:
![03](https://hackmd.io/_uploads/SJ9nJp61bl.png)
![04](https://hackmd.io/_uploads/ry7T16TyZe.png)
![03](https://hackmd.io/_uploads/S1KEXM0k-g.png)
![04](https://hackmd.io/_uploads/SkOV7z0ybe.png)


利用 configure 腳本在 makefile 檔案內部，所設定的交叉編譯設定。
![02](https://hackmd.io/_uploads/B1yoy66ybx.png)




### 4. 使用 make 進行編譯

最後我們使用 make 進行編譯，所產出 sqlite 的 library、執行檔，可以放到指定的ARM 板上進行執行。

首先先使用 make command:
```
make
```

完成後會看到：
```
libsqlite3.la
libsqlite3.a
sqlite3
```

如果先前有設定 **--prefix=/home/joey/sqlite/exe** 並接著執行：
```
make install
```
就會把檔案安裝到你指定的 --prefix 目錄下。

詳細的編譯流程如下
![09](https://hackmd.io/_uploads/rkh0QZC1-l.png)

完成後會在exe檔案夾內存放所建立的目標檔案
![06](https://hackmd.io/_uploads/Sy2ffTpy-e.png)

所有目標檔案的存放位置如下，包含：

```
/home/joey/Unit_C/250412/sqlite/exe/
├── bin/
│   └── sqlite3                ← 執行檔
├── include/
│   └── sqlite3.h              ← 標頭檔
├── lib/
│   ├── libsqlite3.a           ← 靜態 library
│   └── libsqlite3.so          ← 動態 library
└── share/
    └── man/...
```
## SQLite使用範例

### sqliteAP 程式架構
```javascript= 
#include <stdio.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for(int i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}
```

```javascript= 
int main(int argc, char **argv)
{
    sqlite3 *db;	//using database
    char *szErrMsg = 0;
    int nResult;

    if(argc != 3)
    {
        fprintf(stderr, "USAGE: %s DATABASE SQL-STATEMENT\n", argv[0]);
        return(1);
    }

    nResult = sqlite3_open(argv[1], &db);
    if(nResult)
    {
        fprintf(stderr, "ERROR: Can't open database %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }

    nResult = sqlite3_exec(db, argv[2], callback, 0, &szErrMsg);
    if(nResult)
    {
        fprintf(stderr, "ERROR: SQL error %s\n", szErrMsg);
        sqlite3_free(szErrMsg);
    }

    sqlite3_close(db);
    return 0;
}
```
---
### 編譯 sqliteAP 程式

在編譯 sqliteAP 時所使用的相關 Command：
```javascript
aarch64-linux-gnu-gcc sqliteAP.c -o sqliteAP.exe \
    -I./exe/include/ \
    -L./exe/lib \
    -lsqlite3
```
所建立的目標檔案格式
```
project/
├── sqliteAP.c
└── exe/
    ├── include/
    │   └── sqlite3.h
    └── lib/
        ├── libsqlite3.a
        └── libsqlite3.so
```

如果在編譯時改成使用靜態連結：
```
aarch64-linux-gnu-gcc sqliteAP1.c -o sqliteAP.exe \
    -I./exe/include \
    -L./exe/lib \
    -Wl,-Bstatic -lsqlite3 -Wl,-Bdynamic
```

或最簡單的：
```
aarch64-linux-gnu-gcc sqliteAP1.c -o sqliteAP.exe \
    -I./exe/include -L./exe/lib -static -lsqlite3
```

這樣產出的執行檔會把 libsqlite3.a 直接嵌入執行檔中，
不再需要動態 .so 檔。

（⚠️ 缺點：執行檔會比較大，但最方便部署。）

相關參數說明:
| 參數                      | 意義                                              |
| ----------------------- | ----------------------------------------------- |
| `aarch64-linux-gnu-gcc` | 指定交叉編譯器（ARM 64-bit）                             |
| `sqliteAP.c`           | 你的應用程式原始碼                                       |
| `-o sqliteAP.exe`      | 輸出執行檔名稱                                         |
| `-I./exe/include/`      | 告訴編譯器去這裡找 `sqlite3.h`                           |
| `-L./exe/lib`           | 告訴 linker 去這裡找 `libsqlite3.a` 或 `libsqlite3.so` |
| `-lsqlite3`             | 連結 SQLite 函式庫                                   |

編譯完成後會得到 **sqliteAP.exe**，此執行檔可以與先前所建立的 SQLite 資料庫一起在 OrangePi 上使用。

---

### 執行 sqlite3 
在先前的步驟中，我們已經建立完成可以在 ARM 版上使用的 sqlite3 資料庫以及  spliteAP 的資料庫應用程式，接下來我們在 OrangePi 上執行 sqlite3 以及 spliteAP.exe，測試功能是否正常。

首先我們先移植 sqlite3 資料庫，進行使用測試。

#### 1. 確認目前 SQLite 狀態

在OrangePi上執行sqlite3的執行檔，應該看到：

```
sqlite>
```

這是 SQLite 的互動式命令列。

如果你看到「Connected to a transient in-memory database」
代表目前是在「記憶體暫存資料庫」，關掉 SQLite 之後資料就會消失。

#### 2. 建立或打開一個資料庫 .db 檔案

為了在關掉 SQLite 之後確保所輸入的資料不會消失，我們可以建立一個資料庫 .db 檔案

在 SQLite 提示符下輸入：
```
sqlite> .open estbltest.db
```

這樣會在目前目錄下建立一個實體檔案 estbltest.db，
接下來所有的操作（CREATE / INSERT）都會存入這個檔案。

#### 3. 建立資料表並插入資料

在 SQLite 提示符下輸入：
```
sqlite> CREATE TABLE EstblTest (
    ID TEXT,
    Name TEXT
);
```


```
sqlite> INSERT INTO EstblTest VALUES ('E001', 'Joey');
sqlite> INSERT INTO EstblTest VALUES ('E002', 'Alice');
```

#### 4. 查詢確認資料是否寫入


```
sqlite> SELECT * FROM EstblTest;
```
輸出應該會是：
```
E001|Joey
E002|Alice
```

詳細的測試流程步驟如下圖所示，到目前為止我們已經可以確保 sqlite3 是可以正確的寫入資料並且進行讀取的。

![07](https://hackmd.io/_uploads/rkj0106y-e.png)

---

### 執行 sqliteAP

#### 1. 使用 SqliteAP 直接執行 SQL 語句

最後我們可以使用 sqliteAP 直接執行 SQL 語句，來完成自動化存取指定資料庫的功能，先前所編譯的 sqliteAP 的行為主要是:
```
./sqliteAP1 <資料庫檔案> "<SQL指令>"
```
簡單來說：
* 第 1 個參數是資料庫檔案，例如 estbltest.db
* 第 2 個參數是要執行的 SQL 語句（要用引號包起來）

程式會去：
1. 開啟指定的資料庫
2. 執行指定 SQL 語句
3. 呼叫 callback() 印出查詢結果
4. 完成執行 SQLite 語句功能

在執行之前暫時設定環境變數
```
export LD_LIBRA/orangepi/DrvExms/sqlite/exe/lib:$LD_LIBRARY_PATH
```
或是使用靜態連結，不需要 .so

#### 2. 針對資料庫執行 SQL 語句

接下來我們使用 sqliteAP.exe 針對 estbltest.db 執行查詢指令
```javascript
./sqliteAP.exe estbltest.db "SELECT * FROM EstblTest;"
```
結果會顯示：
```javascript
ID = E001
Name = Joey

ID = E002
Name = Alice
```
執行成果如下:
![08](https://hackmd.io/_uploads/HkjpQzRJbx.png)

#### 3. 相關可使用的 SQL 語法
* 查詢資料
    ```
    ./sqliteAP1 estbltest.db "SELECT * FROM EstblTest;"
    ```  
* 新增資料
    ```
    ./sqliteAP1 estbltest.db "INSERT INTO EstblTest VALUES('E003','Bob');"
    ```
* 刪除或修改資料
    ```
    ./sqliteAP1 estbltest.db "DELETE FROM EstblTest WHERE ID='E001';"
    ./sqliteAP1 estbltest.db "UPDATE EstblTest SET Name='Tom' WHERE ID='E002';"
    ```

到目前為止我們已經完成了 SQLite 資料庫移植到 OrangePi 上的範例了，並且可以透過 sqliteAP 輸入 API 來完成資料庫自動化的存取功能。
