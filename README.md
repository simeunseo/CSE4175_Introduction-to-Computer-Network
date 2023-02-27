# CSE4175-Introduction-to-Computer-Network
서강대학교 기초컴퓨터네트워크(2022-1) 과제입니다.

## HW01
Cyclic redundancy check (CRC)를 이용하여 오류를 검출한다.
- encoder : CRC를 이용하여 dataword를 codeword로 변환
```
./crc_encoder input_file output_file generator dataword_size
```
- decoder : codeword를 dataword로 복원하면서 오류가 있으면 검출
```
./crc_decoder input_file output_file result_file generator dataword_size
```

## HW02
Distance Vector와 Link State routing algorithm을 이용하여 각 라우터의 라우팅 테이블을 생성하고 네트워크의 변화에 따라 라우팅 테이블을 업데이트한다.
- linkstate : Link State routing algorithm을 이용하여 라우팅 테이블 관리
```
./linkstate topologyfile messagesfile changesfile
```
- distvec : Distance Vector routing algorithm을 이용하여 라우팅 테이블을 관리
```
./distvec topologyfile messagesfile changesfile
```
