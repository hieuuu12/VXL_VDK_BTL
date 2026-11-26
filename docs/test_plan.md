# Test plan & Test log — BTL CO3009 (phụ trách: B, Gantt 3.4)

Cách dùng: in ra hoặc mở trên điện thoại khi lên **LAB 3 (T7 28/11)**. Làm lần lượt từng ca, ghi **Kết quả thực tế** và **PASS/FAIL**. Ca nào FAIL: tạo một issue trên GitHub (ghi số ca), rồi điền số issue vào cột Ghi chú.
Điều kiện xong của mốc M3 là **PASS ≥ 90% số ca** (ít nhất 19/21).

Thời gian mặc định khi test: **Đỏ 5 s, Xanh 3 s, Vàng 2 s** (nếu không ghi khác).
Bản đồ nút: B1 = đổi chế độ, B2 = pha kế (MANUAL) / chọn đèn (TUNING), B3 = +1 s, B4 = −1 s, USER = Lưu.

## A. Chế độ TỰ ĐỘNG

| # | Ca kiểm thử | Các bước | Kết quả mong đợi | Thực tế | P/F | Ghi chú |
|---|---|---|---|---|---|---|
| 1 | Khởi động | Cấp điện / nhấn RESET | Cả 2 tuyến ĐỎ khoảng 1 s, sau đó Đèn1 ĐỎ – Đèn2 XANH; LCD hiện chế độ AUTO | | | |
| 2 | Đúng thứ tự pha | Quan sát 3 chu kỳ | R1G2 → R1Y2 → G1R2 → Y1R2 → lặp lại | | | |
| 3 | Đúng thời gian | Bấm đồng hồ điện thoại cho từng pha | Xanh 3 s, Vàng 2 s, Đỏ 5 s (sai số ≤ 0,1 s) | | | |
| 4 | Đếm ngược trên LCD | Quan sát LCD | Số giây của 2 tuyến giảm dần mỗi giây, về 1 rồi đổi màu; tuyến ĐỎ đếm từ 5 | | | |
| 5 | Chạy dài | Để chạy liên tục 30 phút | Không treo, không lệch thứ tự, LED PA5 (heartbeat) vẫn nháy đều | | | |

## B. Chế độ BẰNG TAY

| # | Ca kiểm thử | Các bước | Kết quả mong đợi | Thực tế | P/F | Ghi chú |
|---|---|---|---|---|---|---|
| 6 | Vào MANUAL giữ nguyên pha | Đang R1G2 (AUTO), nhấn B1 | Đèn KHÔNG đổi màu, LCD hiện MANUAL | | | |
| 7 | B2 chuyển pha | Nhấn B2 bốn lần, mỗi lần cách nhau > 1 s | R1G2 → R1Y2 → G1R2 → Y1R2 → R1G2 | | | |
| 8 | Khoá 1 s | Nhấn B2 hai lần liên tiếp thật nhanh | Chỉ chuyển 1 pha | | | |
| 9 | Nhấn liên tục | Bấm B2 liên tục thật nhanh trong 10 s | Không lúc nào 2 tuyến cùng XANH; mỗi pha sáng ≥ 1 s; LED PC9 không sáng | | | |
| 10 | Giữ B2 | Giữ B2 trong 3 s | Chỉ chuyển đúng 1 pha (không tự nhảy liên tục) | | | |
| 11 | Đổi chế độ lúc đèn VÀNG | Trong AUTO, đúng lúc R1Y2 thì nhấn B1 | MANUAL giữ R1Y2; nhấn B2 → G1R2 (không bỏ qua vàng) | | | |

## C. Chuyển chế độ (phối hợp với A và C)

| # | Ca kiểm thử | Các bước | Kết quả mong đợi | Thực tế | P/F | Ghi chú |
|---|---|---|---|---|---|---|
| 12 | Vòng chế độ | Nhấn B1 ba lần | AUTO → MANUAL → TUNING → AUTO, LCD đổi tên chế độ | | | |
| 13 | Về AUTO luôn qua ALL_RED | Từ TUNING nhấn B1 | Cả 2 tuyến ĐỎ khoảng 1 s rồi mới R1G2 | | | |
| 14 | Nhấn B1 dồn dập | Bấm B1 liên tục 10 s | Không treo, không lúc nào 2 tuyến cùng XANH, không còn cờ nút “thừa” sau khi dừng | | | |
| 15 | Nút trong chế độ khác không gây tác dụng | Ở AUTO nhấn B2, B3, B4, USER | Đèn chạy bình thường, không nhảy pha | | | |

## D. Chỉnh thời gian (phần của C — B chỉ kiểm tra ảnh hưởng lên AUTO)

| # | Ca kiểm thử | Các bước | Kết quả mong đợi | Thực tế | P/F | Ghi chú |
|---|---|---|---|---|---|---|
| 16 | Lưu giá trị hợp lệ | Chỉnh Xanh = 4, Vàng = 2, Đỏ = 6, nhấn USER, về AUTO | AUTO chạy Xanh 4 s, Vàng 2 s, Đỏ 6 s | | | |
| 17 | Nhập sai | Chỉnh Đỏ = 9, Xanh = 3, Vàng = 2 (9 ≠ 3+2), nhấn USER | LCD báo lỗi 2 s, giữ giá trị cũ; AUTO vẫn chạy 5–3–2 | | | |
| 18 | Thoát khi chưa lưu | Chỉnh giá trị rồi nhấn B1 (không nhấn USER) | AUTO chạy với giá trị cũ | | | |

## E. An toàn & tĩnh

| # | Ca kiểm thử | Các bước | Kết quả mong đợi | Thực tế | P/F | Ghi chú |
|---|---|---|---|---|---|---|
| 19 | Ép vi phạm an toàn | Bản build test: gọi `traffic_set_pair(TL_GREEN, TL_GREEN)` | Cả 2 tuyến ĐỎ, LED PC9 sáng | | | |
| 20 | Kiểm tra tĩnh | `bash tools/check_static.sh` + Build với -Wall | Tất cả PASS; tab Problems 0 warning | | | |
| 21 | Test PC | `cd test_pc && make` | `0 FAIL` | | | |

**Tổng:** ____ / 21 PASS  →  ____ %     Người test: ________   Ngày: ____/____/2026
