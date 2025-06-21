# Lab Helper

This project is came from one person who want to make a large digit number calculator. Then in later, he make this project and share with his friend.

### What this project Goal
This project we are make for who want to calculate a complicated, large number with any sizes of computer. For help with the project success faster and easier.


---

### How to Compile this Project

This project you must to install CMake to compile it. And IDE Tool like visual studio code to help you manage and coding easy.

If you have only CMake, run this command.

```sh
cmake -S . -B build
cmake --build build
```

---

### Flow of work on 21 / 06 / 2025

#### Addtion Function
```
[เริ่มต้น]
   |
   v
[เปิดไฟล์ 1.txt, 2.txt, OutputFile]
   |
   v
[วนลูป ขณะที่ InputFile_1 != NULL และ InputFile_2 != NULL]
   |
   +--> [ถ้า filesize1 < 18?]
   |        | 
   |        v
   |   [อ่านทั้งหมดที่เหลือใน InputFile_1]
   |        |
   |        v
   |   [ตั้ง last_num1 = true]
   |        |
   |        v
   |   [filesize1 = 0]
   |        |
   |        v
   |   [else]
   |        |
   |        v
   |   [เลื่อนไป 18 หลักท้ายสุดใน InputFile_1]
   |   [อ่าน 18 หลัก]
   |   [filesize1 -= 18]
   |   [ตัดท้ายไฟล์ออก 18 หลักด้วย ftruncate]
   |
   +--> [ทำเหมือนกันกับ InputFile_2]
   |
   +--> [ถ้า filesize1 == 0 ปิด InputFile_1, ตั้งเป็น NULL]
   +--> [ถ้า filesize2 == 0 ปิด InputFile_2, ตั้งเป็น NULL]
   |
   v
[บวกตัวเลขทั้งสอง + carry]
   |
   v
[ถ้าผลรวม >= 18 หลัก]
   |
   +--> [ตั้ง carry = 1, ลบ max_sum ออก]
   |
   v
[ถ้าผลรวมไม่ครบ 18 หลักและยังไม่ใช่ก้อนสุดท้าย]
   |
   +--> [เติม 0 ข้างหน้าให้ครบ 18 หลัก]
   |
   v
[เขียนผลรวมลง OutputFile]
   |
   v
[วนกลับไปเช็ค InputFile_1 และ InputFile_2]
   |
   v
[จบลูปและปิดไฟล์ทั้งหมด]
   |
   v
[สิ้นสุด]
```