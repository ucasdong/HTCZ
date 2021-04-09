/// 产生pps  and other  pulse
`define simu
module pps_gen(
input aclk,//100M
input aresetn,
input [2:00] pps_sel_i,
input pps_en_i,//  1'b1

output  reg pps_out = 'b1,
output  reg pulse_out = 'b1


);
`ifdef simu
parameter s_14_41 = 64'd14410 ;
parameter s_5     = 64'd5000;
parameter s_6     = 64'd6000;
parameter s_7     = 64'd7000;
parameter s_8     = 64'd8000;
parameter s_9     = 64'd9000;
parameter s_11    = 64'd11000;
parameter s_13    = 64'd13000;
parameter us_20   = 64'd20;
parameter s_095   = 64'd950;
parameter s_3_567 = 64'd3567;
parameter s_4_594 = 64'd4594;
`else

parameter s_14_41 = 64'd1441000000;
parameter s_5     = 64'd500000000;
parameter s_6     = 64'd600000000;
parameter s_7     = 64'd700000000;
parameter s_8     = 64'd800000000;
parameter s_9     = 64'd900000000;
parameter s_11    = 64'd1100000000;
parameter s_13    = 64'd1300000000;
parameter us_20   = 64'd2000;
parameter s_095   = 64'd95000000;
parameter s_3_567 = 64'd356700000;
parameter s_4_594 = 64'd459400000;
`endif


reg [63:00]	time_cnt;
reg [63:00] time_end = 'b0;
reg [3:0]	st_cnt;
always@(posedge aclk,negedge aresetn)
 if(aresetn == 'b0)begin
    st_cnt   <= 'b0;
	time_cnt <= 'b0;end
 else begin
 case(st_cnt)
  'd0:begin
       time_cnt <= 'b0;
       if     (pps_sel_i == 'd0)
		  time_end <= s_14_41;
	   else if(pps_sel_i == 'd1)
		  time_end <= s_5;
	   else if(pps_sel_i == 'd2)
		  time_end <= s_6;		  
	   else if(pps_sel_i == 'd3)
		  time_end <= s_7;		  
	   else if(pps_sel_i == 'd4)
		  time_end <= s_8;
	   else if(pps_sel_i == 'd5)
		  time_end <= s_9;	   
	   else if(pps_sel_i == 'd6)
		  time_end <= s_11;
	   else  
		  time_end <= s_13;//初始化
      if(pps_en_i) st_cnt <= 'd1;
	  
	  else st_cnt <= 'd0;
      end
	'd1:begin if(time_cnt == time_end - 1'b1) begin
	            st_cnt <= 'b0;
				time_cnt <= 'b0;end
			  else 
			    time_cnt <= time_cnt + 1'b1;
       	end  
	  
	  
 default:st_cnt <= 'b0;
 endcase
 end
 always@(posedge aclk,negedge aresetn)
	if(aresetn == 'b0)
	 pps_out <= 'b1;
	else if(time_cnt >'d1 && time_cnt <us_20 + 'd1)
	 pps_out <= 'b0;
	else
	 pps_out <= 'b1;
	 
 always@(posedge aclk,negedge aresetn)
  if(aresetn == 'b0)
	 pulse_out <= 'b1;
  else if(time_cnt > s_095 && time_cnt < s_095 + us_20)
	 pulse_out <= 'b0;
  else if(time_cnt > s_3_567 && time_cnt < s_3_567 + us_20)
	 pulse_out <= 'b0;
  else if(time_cnt > s_4_594 && time_cnt < s_4_594 + us_20)
	 pulse_out <= 'b0;	 
  else
	 pulse_out <= 'b1;

endmodule
