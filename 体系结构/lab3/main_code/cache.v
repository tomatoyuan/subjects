`timescale 1ns / 1ps

module cache(
    input            clk             ,  // clock, 100MHz
    input            rst             ,  // active low

    //  Sram-Like接口信号定义:
    //  1. cpu_req     标识CPU向Cache发起访存请求的信号，当CPU需要从Cache读取数据时，该信号置为1
    //  2. cpu_addr    CPU需要读取的数据在存储器中的地址,即访存地址
    //  3. cache_rdata 从Cache中读取的数据，由Cache向CPU返回
    //  4. addr_ok     标识Cache和CPU地址握手成功的信号，值为1表明Cache成功接收CPU发送的地址
    //  5. data_ok     标识Cache和CPU完成数据传送的信号，值为1表明CPU在本时钟周期内完成数据接收
    input         cpu_req      ,    //由CPU发送至Cache
    input  [31:0] cpu_addr     ,    //由CPU发送至Cache
    output [31:0] cache_rdata  ,    //由Cache返回给CPU
    output        cache_addr_ok,    //由Cache返回给CPU
    output        cache_data_ok,    //由Cache返回给CPU

    //  AXI接口信号定义:
    //  Cache与AXI的数据交换分为两个阶段：地址握手阶段和数据握手阶段
    output [3 :0] arid   ,              //Cache向主存发起读请求时使用的AXI信道的id号，设置为0即可
    output [31:0] araddr ,              //Cache向主存发起读请求时所使用的地址
    output        arvalid,              //Cache向主存发起读请求的请求信号
    input         arready,              //读请求能否被接收的握手信号

    input  [3 :0] rid    ,              //主存向Cache返回数据时使用的AXI信道的id号，设置为0即可
    input  [31:0] rdata  ,              //主存向Cache返回的数据
    input         rlast  ,              //是否是主存向Cache返回的最后一个数据
    input         rvalid ,              //主存向Cache返回数据时的数据有效信号
    output        rready                //标识当前的Cache已经准备好可以接收主存返回的数据  
);

    /*-----------state-----------*/
    parameter idle    = 0;
    parameter run     = 1;
    parameter sel_way = 2;
    parameter miss    = 3;
    parameter refill  = 4;
    parameter finish  = 5;
    parameter resetn  = 6;

    reg [2:0] state; 

    /* DFA */
    always @(posedge clk) begin
        if (!rst) begin
            state <= idle;
        end
        else begin
            /*TODO：根据设计的自动机的状态转移规则进行实现*/
        end
    end



    /*-----------RESETN-----------*/
    
    // TODO: 设计一个计数器，从state = RESETN开始从0计数，每一拍加一，当记满128拍后说明初始化完成



    /*-----------Request Buffer-----------*/
    reg        reg_req ;
    reg [31:0] reg_addr;
    reg [19:0] reg_tag    ;
    reg [6 :0] reg_index  ;
    reg [4 :0] reg_offset ;
    // 根据自己设计自行增删寄存器

    always @(posedge clk) begin
        if (!resetn) begin
            /*TODO: 初始化寄存器*/
        end
        else if (/* Request Buffer 更新条件 */) begin
            /*TODO: 更新寄存器*/
        end
    end
    


    /*-----------LRU-----------*/
    reg [`ICACHE_GROUP_NUM-1:0] lru;
    // 根据自己设计自行增删寄存器

    // LRU Update
    /*TODO: 在命中的 RUN 状态和不命中的 MISS 状态进行 LRU 的更新*/

    // LRU Select Way
    /*TODO: 在 SEL_WAY 状态进行选路*/
    


    /*-----------Refill-----------*/
    /*TODO: 设计一个计数器，用于记录当前refill的指令个数



    /*-----------tagv && data-----------*/
    wire [1 :0] tagv_wen;
    wire [6 :0] tagv_index;
    wire [19:0] tagv_tag;
    wire [31:0] valid_wdata;
    wire [1:0] hit_array;

    assign tagv_wen[0] = ;
    assign tagv_wen[1] = ;
    assign tagv_index  = ;
    assign tagv_tag    = ;
    assign valid_wdata = ;

    wire [31 :0] data_wen [1:0];
    wire [6  :0] data_index;
    wire [4  :0] data_offset;
    wire [255:0] data_wdata;
    wire [31 :0] data_rdata [1:0];

    assign data_wen[0] = ;
    assign data_wen[1] = ;
    assign data_index  = ;
    assign data_offset = ;
    assign data_wdata  = ;
    generate
        genvar j;
        for (j = 0 ; j < 2 ; j = j + 1) begin
            icache_tagv Cache_TagV (
                .clk        (clk         ),
                .wen        (tagv_wen[j] ),
                .index      (tagv_index  ),
                .tag        (tagv_tag    ),
                .valid_wdata(valid_wdata ),
                .hit        (hit_array[j])
            );
            icache_data Cache_Data (
                .clk          (clk          ),
                .wen          (data_wen[j]  ),
                .index        (data_index   ),
                .offset       (data_offset  ),
                .wdata        (data_wdata   ),
                .rdata        (data_rdata[j])
            );
        end
    endgenerate

    /*------------ CPU<->Cache -------------*/
    assign cache_addr_ok = ;
    assign cache_data_ok = ;

    // select way
    assign cache_rdata = ;

    /*-----------------AXI------------------*/
    // Read
    assign arid    = 4'd0;
    assign arvalid = ;  
    assign araddr  = ;                       
    
    assign rready  = ;    
endmodule

