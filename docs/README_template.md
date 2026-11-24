# CO3009 – Hệ thống đèn giao thông 2 tuyến trên XNUCLEO-F103RB

> Nhóm: XX · Thành viên: A (MSSV), B (MSSV), C (MSSV) · Môn: Vi xử lý – Vi điều khiển, HK1 2026–2027

## 1. Mô tả dự án
Hệ thống điều khiển đèn giao thông ở ngã tư 2 tuyến đường, chạy trên một scheduler O(1) với nhịp 10 ms. Hệ thống có 3 chế độ:
- **AUTO**: 2 tuyến Đỏ – Xanh – Vàng luân phiên theo thời gian cài đặt (mặc định Đỏ 5 s, Xanh 3 s, Vàng 2 s); LCD hiện số giây đếm ngược.
- **MANUAL**: chuyển pha đèn bằng nút B2, theo chuỗi an toàn và có khoá 1 s.
- **TUNING**: chỉnh thời gian Đỏ/Xanh/Vàng; ràng buộc Đỏ = Xanh + Vàng.

LCD còn hiện nhiệt độ và độ ẩm đọc từ cảm biến DHT20 (cập nhật mỗi 2 s).
**Bất biến an toàn:** tại mọi thời điểm ít nhất một tuyến ĐỎ. Nếu có lệnh vi phạm, hệ thống ép cả 2 tuyến ĐỎ và bật LED PC9.

## 2. Cấu trúc thư mục
```
Core/
  Inc/, Src/
    main.c                 khởi tạo, bảng task, vòng SCH_Dispatch_Tasks
    scheduler.*            scheduler O(1)                          (A)
    global.*               biến dùng chung (thời gian đèn...)      (A)
    fsm_mode.*             FSM chuyển chế độ                       (A)
    software_timer.*       timer phần mềm 10 ms                    (B)
    button.*               chống dội, nhấn giữ, auto-repeat        (B)
    traffic_light.*        xuất đèn + kiểm tra bất biến an toàn    (B)
    fsm_automatic.*        chế độ tự động                          (B)
    fsm_manual.*           chế độ bằng tay                         (B)
    lcd.*, dht20.*         driver I2C                              (C)
    display.*              các màn hình LCD                        (C)
    fsm_tuning.*           chế độ chỉnh giờ                        (C)
docs/                      sơ đồ FSM, test plan, design.md
test_pc/                   test các module trên PC bằng gcc (HAL giả lập)
tools/check_static.sh      kiểm tra tĩnh (không có HAL trong fsm_*.c ...)
```

## 3. Build và nạp
1. Cài **STM32CubeIDE** phiên bản X.Y.Z (cả nhóm dùng cùng một phiên bản).
2. `git clone <link-repo>`, sau đó vào File → Import → *Existing Projects into Workspace* và chọn thư mục repo.
3. Cắm mạch XNUCLEO-F103RB qua USB (ST-LINK).
4. Nhấn **Build** (biểu tượng búa), rồi **Run** (biểu tượng ▶). Nếu được hỏi cấu hình, chọn *STM32 C/C++ Application*, debug probe ST-LINK, interface **SWD**.
5. Nếu nạp lỗi: kiểm tra `.ioc` → SYS → Debug = **Serial Wire**, rồi làm theo video https://www.youtube.com/watch?v=qxaW6FMiVWQ.

Chạy test trên PC (không cần mạch): `cd test_pc && make` (cần gcc; trên Windows dùng MSYS2 hoặc WSL).

## 4. Cách vận hành
| Kết nối | Vị trí |
|---|---|
| Đèn tuyến 1 / tuyến 2 | D3–D4 / D5–D6 |
| Nút B1, B2 / B3, B4 | D7–D8 / D9–D10 |
| LCD + DHT20 | khe I2C màu đỏ |

| Nút | AUTO | MANUAL | TUNING |
|---|---|---|---|
| B1 | → MANUAL | → TUNING | → AUTO (qua ALL_RED 1 s) |
| B2 | – | pha kế (khoá 1 s) | chọn đèn Đỏ/Xanh/Vàng |
| B3 / B4 | – | – | +1 s / −1 s (giữ để tăng/giảm nhanh) |
| USER | – | – | Lưu (sai ràng buộc → báo lỗi 2 s) |

Đèn báo: LED PA5 nháy 0,5 s = hệ thống đang chạy; LED PC9 sáng = đã chặn một lệnh vi phạm an toàn.
