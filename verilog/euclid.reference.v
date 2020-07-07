module euclid(m_in, n_in, gcd, clk, rst);
  input  [15:0] m_in;
  input  [15:0] n_in;

  output [15:0] gcd;
  reg    [15:0] gcd;

  input  clk;
  input  rst;

  reg [15:0] m, m_next;
  reg [15:0] n, n_next;
  reg done, done_next;
  reg [15:0] factor, factor_next;

  reg [1:0] state, state_next;

  parameter s0 = 2'd0, s1 = 2'd1, s2 = 2'd2;

  always @(posedge clk)
   if (rst) begin
     n      <= 16'd0;
     m      <= 16'd0;
     done   <= 1'd0;
     factor <= 16'd0;
     state  <= s0;
   end else begin 
     n      <= n_next;
     m      <= m_next;
     done   <= done_next;
     factor <= factor_next;
     state  <= state_next;
   end

  always @(*) begin
    n_next      <= n;       // default reg assignment
    m_next      <= m;       // default reg assignment
    done_next   <= done;    // default reg assignment
    factor_next <= factor;  // default reg assignment

    gcd         <= 16'd0;   // default output assignment

    case (state)

      s0: begin
          m_next      <= m_in;
          n_next      <= n_in;
          factor_next <= 16'd0;
          done_next   <= 1'd0;
          gcd         <= 16'd0;

          // -> s1
          state_next <= s1;
          end

      s1: begin

          if (done) begin  
             gcd <= ((m > n) ? m : n) << factor;

             // -> s2
             state_next <= s1;

          end else begin

             if (m[0] & n[0]) begin 

                // reduce
                m_next    <= (m >= n) ? m - n : m;
                n_next    <= (n >  m) ? n - m : n;

                // outidle
                gcd       <= 16'd0;
                done_next <= ((m == 0) | (n == 0));

                // -> s1
                state_next <= s1;

             end else begin

                if (m[0] & ~n[0]) begin 

                   // shiftn
                   n_next <= n >> 1;

                   // outidle
                   gcd       <= 16'd0;
                   done_next <= ((m == 0) | (n == 0));                   

                   // -> s1
                   state_next <= s1;

                end else begin

                   if (~m[0] & n[0]) begin 

                      // shiftm
                      m_next <= m >> 1;

                      // outidle
                      gcd       <= 16'd0;
                      done_next <= ((m == 0) | (n == 0));                   

                      // -> s1
                      state_next <= s1;

                   end else begin 

                      // shiftn
                      n_next <= n >> 1;

                      // shiftm
                      m_next <= m >> 1;

                      // shiftf
                      factor_next <= factor + 1;

                      // outidle
                      gcd       <= 16'd0;
                      done_next <= ((m == 0) | (n == 0));                   

                      // -> s1
                      state_next <= s1;

                   end
                end
              end 
            end
          end

      s2: begin

          // outidle
          gcd       <= 16'd0;
          done_next <= ((m == 0) | (n == 0));                   

          end

      default: begin
               state_next <= s0;  // jump back to init           
               end
    endcase
  end

endmodule

module test_euclid(m, n, clk, rst);
  output [15:0] m;
  output [15:0] n;
  // reg    [15:0] m; -- only when m,n assigned in sfg
  // reg    [15:0] n;
  input clk;
  input rst;
 
  assign m = 16'd2322;
  assign n = 16'd654;
    
endmodule

module euclid_sys(clk, rst);
  wire [15:0] m;
  wire [15:0] n;
  wire [15:0] gcd;
  input clk;
  input rst;

  euclid M1(m, n, gcd, clk, rst);
  test_euclid M2(m, n, clk, rst);

endmodule

module top;
  reg rst;
  reg clk;
  
  euclid_sys M1(clk, rst);
  
  initial begin
    rst = 1;
    clk = 0;
    #100
    rst = 0;
  end
  
  always #10 clk = ~clk;

endmodule
 
