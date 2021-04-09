`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/04/09 19:53:05
// Design Name: 
// Module Name: test
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module test(

    );
	wire pps_out;
	wire pulse_out;
	reg aclk;
	reg aresetn;
	reg [2:00]  pps_sel_i;
	reg pps_en_i;
	
	initial begin
	aclk = 'b0;
	aresetn = 'b0;
	pps_sel_i = 'b0;
	pps_en_i = 'b0;
	#100;
	aresetn = 'b1;
	#100;
	
	pps_en_i = 'b1;
	#1000000;
	pps_sel_i = 'b1;
	end
	always #5 aclk = ~aclk;
	
pps_gen pps_gen_inst(
      .aclk			(aclk)	,//100M
      .aresetn		(aresetn)	,
      .pps_sel_i	(pps_sel_i)	,
      .pps_en_i		(pps_en_i)	,//  1'b1

      .pps_out  	(pps_out)	,
      .pulse_out    (pulse_out)


);
endmodule
