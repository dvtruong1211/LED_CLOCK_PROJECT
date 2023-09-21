YÊU CẦU KĨ THUẬT 
THIẾT KẾ ĐỒNG HỒ RTC HIỂN THỊ LED7SEG CỠ LỚN

I. Hardware
1.1 Led 7 thanh cỡ lớn
- tự thiết kế theo kích thước file led7seg.docx kèm theo
- Khung cắt từ tấm fomex trắng
- Hiển thị được 6 led 7 thanh, 2 led dấu 2 chấm
- Các led đều là loại led RGB 5mm, anot chung (mỗi thanh 1 led)
- Đấu dây: các led RGB trong 1 led 7 thanh thì đấu chung chân anot, các chân katot của cả 6 led 7 thanh đấu chung theo từng màu. Như vậy là có 6 chân + chung, mỗi chân cấp nguồn 1 led7 thanh, 21 chân -, cứ 7 chân ứng với 1 màu của cả 6 led
	- led2 chấm đấu chung hết các chân lại (coi như 1 led RGB
	
1.2 PCB 
- có schematic kèm theo
- MCU chọn là kit stmF103C8T6 (tạm thời đang thay bằng chip F3)
- Sử dụng 74138 để quét led, 3 hc74595 để điều khiển 21 chân data ở trên, pha màu có thể sử dụng các chân data này
- Module RTC là ds3231 (có đo nhiệt độ)
- Có còi chip
- Có thu IR: đang sử dụng remote hồng ngoại 21 phím
- Có Phát RF: sử dung module phát RF (có thể là h34C ) 315 hoặc 433Mhz tùy chọn, loại chưa mã hóa
- nguồn cấp cho led 7 thanh, IC đệm và còi chip là 5V qua trans. Nguồn cấp cho các phần còn lại là 3.3V (lấy được từ kit)
- 4 nút nhấn để cài đặt: MODE, PLUS, MINUS, OK

II. Software và User interface

1. Hiển thị
- bình thường led sẽ hiển thị giờ phút giây đọc từ module RTC, led2 chấm nhấp nháy 500ms (màu default).

2. Cài đặt
2.1 Học lệnh IR
- Nhấn giữ nút nhấn MODE trong khoảng 5s, khi còi kêu tiếng dài hoặc màn hình hiển thị thay đổi thì bắt đầu chế độ học lệnh.
- mặc định sẽ hiển thị chỉ số cho mã phím thứ 0 hiển thị ở led SECOND (led5, led6), led 2 chấm nhấp nháy màu đỏ (báo đang sẵn sàng học lệnh), 
- nhấn phím  ở remote hồng ngoại, nếu học được mã lệnh thành công thì led 2 chấm sẽ chuyển màu xanh và dừng nhấp nháy.
- để chuyển sang chỉ số mã lệnh tiếp theo hoặc trước đó thì nhấn nút PLUS hoặc MINUS, nhấn giữ để chuyển nhanh hơn
- Tạm thời sẽ học tối đa 21 phím, chức năng như dưới ứng với thứ tự 0->20
IR_KEY_0 = 0,
	IR_KEY_1,
	IR_KEY_2,
	IR_KEY_3,
	IR_KEY_4,
	IR_KEY_5,
	IR_KEY_6,
	IR_KEY_7,
	IR_KEY_8,
	IR_KEY_9,
	IR_KEY_MODE,
	IR_KEY_UP,
	IR_KEY_DOWN,
	IR_KEY_RIGTH,
	IR_KEY_LEFT,
	IR_KEY_OK,
	IR_KEY_POWER,
	IR_KEY_MUTE,
	IR_KEY_SW1224,
	IR_KEY_TEMP,
	IR_KEY_AUTO_COLOR,

- thời gian cho mỗi lần học là 60s, nếu không nhận được mã sau khoảng thời gian này sẽ tự động trở về trạng thái hiểnthị thời gian, sau khi cài xong thì có thể sử dụng phím remote để cài đặt các thông số dưới đây
- các chức năng sau chỉ sử dụng với remote
- Nhấn phím Power ở remote thì sẽ tắt 6 led7 thanh. Led2 nhấp nháy chậm 1000ms, còi kêu dài
- bật lại cũng nhấn phím Power hoặc nút nhấn MODE, còi kêu 2 tiếng, hiển thị thời gian bình thường
- Nhấn phím SW1224 thì thay đổi chế độ 12/24h
- NHấn phím TEMP  để hiển thị nhiệt độ, led 2  chấm off. Đồng hồ sẽ hiển thị trong 5 giây sau đó tự động trở về trạng thai bình thường hoặc nhấn phím hoặc Button MODE để trở về 
- Nhấn phím BUZZER để bật tắt tiếng, còi kêu tiếng dài trước khi tắt và kêu 2 tiếng khi bật.
- Nhấn phím COLOR để automatic fade led, có 2 hiệu ứng, fade toàn bộ led và fade từng cặp led. Nhấn phím thêm lần nữa để chuyển hiệu ứng hoặc tắt auto. Lưu ý: việc auto fade này làm thay đổi giá trị color nhưng không lưu vào flash. Có thể sử dụng fade led này trong lúc cài đặt để lưu


2.2 Cài thời gian
- Nhấn button MODE hoặc phím MODE 1 lần để bắt đầu cài thời gian, nhấn thêm 1 lần nữa sẽ chuyển tới cài đặt màu cho led7 thanh
- Bắt đầu cài thời gian:
+ hiển thị giờ,phút, led giờ nhấp nháy 300ms 
+ Nhấn PLUS/MINUS đối với nút nhấn, UP/DOWN đối với remote để thay đổi giá trị đang hiển thị, nhấn giữ để tang giảm nhanh hơn, khi nhấn giữ led sẽ không nhấp nháy, có thể cài bằng phím 0->9 trên remote để nhanh hơn.
+ nhấn nut hoặc phím OK để lưu lại giá trị vừa cài đặt nếu không  khi trở về hoặc chuyển sang led khác sẽ không lưu, sau khi nhấn OK tự động chuyển sang giá trị phút, led phut nhấp nháy, led giờ dừng
+  có thể chuyển sang phút bằng phím LEFT/RIGHT trên remote để nhanh hơn, cài đặt tương tự như giờ.
+ tương tự với cài đặt ngày trong tuần, tháng, ngày trong tháng, năm, led2 tắt. Mặc định năm chỉ cài từ 2000->2099.  Sau khi nhấn OK ở năm tự động trở về hiển thị thời gian bình thường.


2.3 Cài màu
- KHi nhấn MODE -> cài màu.
- ban đầu mặc định hiển thị led giờ với màu hiện tại, sáng hết các thanh, các led còn lại tắt.
- led2 chấm hiển thị cường độ sáng của màu đỏ của led giờ. 
- nhấn PLUS/MINUS đối với nút nhấn, UP/DOWN đối với remote để thay đổi cường độ sáng trên led 2 chấm, led7 thanh sẽ phối màu theo giá trị mới, nhấn giữ để thay đổi nhanh hơn.
- nhấn OK để chuyển sang màu xanh lá, hiển thị cường độ xanh trên led 2 chấm, làm tương tự như màu đỏ, sau đó đến màu blue, nhấn OK tiếp để chuyển sang led phút, hoặc nhấn LEFT/RIGTH để chuyển nhanh hơn.
- LƯU Ý: chỉ khi nhấn OK ở lúc cài màu blue của led giây thì mới lưu vào flash. Nếu không khi reset hoặc cấp nguồn lại sẽ hiển thị giá trị cũ.
Khi không tác động đến remote hoặc nut nhấn thì sau khoảng 30 sẽ tự động thoát khỏi cài đặt trở về trạng thái hiển thị thời gian bình thường.
