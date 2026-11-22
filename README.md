# CO3009 – Bài tập lớn Vi xử lý – Vi điều khiển

**Đề tài:** Hệ thống đèn giao thông 2 tuyến đường trên mạch XNUCLEO-F103RB (STM32F103RB)
**Giảng viên hướng dẫn:** TS. Lê Trọng Nhân · **Lớp:** L0x · **Nhóm:** XX

## Thành viên

| Họ tên | MSSV | GitHub | Phụ trách |
| --- | --- | --- | --- |
| Nguyễn Đức Hiếu | ... | [@hieuuu12](https://github.com/hieuuu12) | ... |
| ... | ... | ... | ... |
| ... | ... | ... | ... |

## Chức năng

- **Chế độ tự động:** 2 tuyến đèn Đỏ – Xanh – Vàng luân phiên theo thời gian cài đặt.
- **Chế độ bằng tay:** chuyển trạng thái đèn bằng nút nhấn.
- **Chế độ chỉnh thời gian:** đổi thời gian Đỏ – Xanh – Vàng bằng nút nhấn, hiển thị trên LCD.
- **Hiển thị môi trường:** đọc DHT20, hiển thị nhiệt độ và độ ẩm trên LCD.
- **Scheduler O(1):** `SCH_Add_Task`, `SCH_Delete_Task`, `SCH_Update` (O(1)), `SCH_Dispatch_Tasks`, hỗ trợ one-shot task.

## Phần cứng

| Thiết bị | Vị trí kết nối |
| --- | --- |
| Module đèn giao thông 1 | D3 – D4 |
| Module đèn giao thông 2 | D5 – D6 |
| Module nút nhấn 1 | D7 – D8 |
| Module nút nhấn 2 | D9 – D10 |
| DHT20, LCD | Khe I2C (màu đỏ) |
| LED / nút USER | PA5, PC9 / PC13 |

## Cấu trúc thư mục (theo thông báo BTL)

```
Core/Inc/   global.h  scheduler.h  software_timer.h  button.h
            traffic_light.h  dht20.h  lcd.h
            fsm_automatic.h  fsm_manual.h  fsm_tuning.h
Core/Src/   các file .c tương ứng
<ten-du-an>.ioc, Drivers/, ...   (do STM32CubeIDE sinh)
README.md
```

## Nhánh

| Nhánh | Dùng để |
| --- | --- |
| `main` | Chỉ chứa bản nộp |
| `dev` | Nhánh chung, gộp phần của mọi người |
| `feature/<ten-phan-viec>` | Mỗi người, mỗi phần việc một nhánh, ví dụ `feature/scheduler`, `feature/button`, `feature/dht20-lcd` |

## Bước 0 – Tạo project CubeIDE (1 người làm, 1 lần)

1. STM32CubeIDE → `File → New → STM32 Project` → thẻ **Board Selector** → **NUCLEO-F103RB**.
2. Mở `.ioc` → `System Core → SYS → Debug` = **Serial Wire**. Hai bước này bắt buộc, nếu sai mạch chỉ nạp được 1 lần.
3. Mở Git Bash trong thư mục project vừa tạo, nối nó với repo rồi đẩy lên `dev`:
   ```bash
   git init -b main
   git remote add origin https://github.com/hieuuu12/VXL_VDK_BTL.git
   git fetch origin
   git checkout dev
   git add -A
   git commit -m "project: tao project CubeMX NUCLEO-F103RB, Serial Wire"
   git push origin dev
   ```

Từ đây **chỉ người này sửa file `.ioc`**. Ai cần đổi chân hay ngoại vi thì nhắn người đó, để tránh conflict khi sinh lại code.

## Cách push phần của mình

```bash
# lần đầu
git clone https://github.com/hieuuu12/VXL_VDK_BTL.git
cd VXL_VDK_BTL
git config user.name  "Ho Ten Cua Ban"
git config user.email "email-dang-ky-github@..."   # phải trùng email GitHub thì commit mới tính cho bạn

# mỗi lần làm phần mới
git checkout dev
git pull
git checkout -b feature/<ten-phan-viec>
# ... code, build thử trong CubeIDE ...
git add <cac-file-cua-minh>
git commit -m "<module>: <viec da lam>"
git push -u origin feature/<ten-phan-viec>
```

Sau đó lên GitHub mở **Pull Request** vào nhánh `dev` và nhờ 1 người khác trong nhóm duyệt rồi merge.

Mở project trong CubeIDE: `File → Import → General → Existing Projects into Workspace` → chọn thư mục repo.

## Quy ước

- Mỗi người commit bằng tài khoản của chính mình. Lịch sử commit là căn cứ đánh giá đóng góp.
- Không push thẳng vào `main`; vào `dev` thì qua Pull Request.
- Commit message dạng `<module>: <việc làm>`, ví dụ `scheduler: add one-shot task support`.
- Tên file chữ thường, nối bằng gạch dưới (`fsm_automatic.c`); tên hàm dạng `<module>_<hành_động>` (`button_read()`, `traffic_set_state()`).
- Code tự viết trong `main.c` chỉ đặt giữa các cặp `USER CODE BEGIN … / USER CODE END …`, nếu không sẽ bị xóa khi sinh lại code.
- Không dùng `HAL_Delay` trong chương trình chính; không gọi HAL trong các file `fsm_*.c`.

## Nộp bài

```bash
git checkout main
git merge dev
git tag v1.0-final
git push origin main --tags
```

Ghi 7 ký tự đầu của commit hash vào bìa báo cáo. Trước khi nộp, đổi repo sang Public hoặc thêm giảng viên làm collaborator.
