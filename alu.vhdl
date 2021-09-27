--
-- UNIDADE LÓGICA ARITMÉTICA EM VHDL
--
--         S2  S1  S0   |     O                      
--   --------------------------------                 
--      0     0     0   |  A  +  B   (Complemento de 2)
--      0     0     1   |  A  -  B   (Complemento de 2)
--      0     1     0   |  A and B    
--      0     1     1   |  A  or B
--      1     0     0   |  A xor B     
--      1     0     1   |  not A
--      1     1     0   |  not B
--      1     1     1   |     Z
--
--   Autor: Lucas da Silva Moreira
--   Curso: Engenharia Eletronica e da Computacao
--   Data: 9 de agosto de 2021
--
--   Primeiro trabalho de Sistemas Digitais
--

  library IEEE;
	use IEEE.std_logic_1164.all;
	use IEEE.std_logic_unsigned.all;
	
	entity alu is port
	(
	    V_SW:    in std_logic_vector(10 downto 0);
		G_LEDG: out std_logic_vector(3 downto 0);
		HEX1: out std_logic_vector(6 downto 0)
	);
	end alu;
	
	architecture hardware of alu is
	signal A  : std_logic_vector(3 downto 0);
	signal B  : std_logic_vector(3 downto 0);
	signal S  : std_logic_vector(2 downto 0);
	signal O  : std_logic_vector(3 downto 0);
	signal H1 : std_logic_vector(6 downto 0);
	begin
	A <= V_SW(3 downto 0);
	B <= V_SW(7 downto 4);
	S <= V_SW(10 downto 8);
	process(A,B,S)
	begin
			case S is
			
			when "000" => O <= A   +   B;
			when "001" => O <= A   +   ((NOT B) + "0001");
			when "010" => O <= A  AND  B;
			when "011" => O <= A   OR  B;
			when "100" => O <= A  XOR  B;
			when "101" => O <= not A;
			when "110" => O <= not B;
			when others => O <= "ZZZZ";
			
			end case;
	end process;
	process(A)
    begin
    case A is
                when "0000"=> H1 <="1111110";
				when "0001"=> H1 <="0110000";
				when "0010"=> H1 <="1101101";
				when "0011"=> H1 <="1111001";
				when "0100"=> H1 <="0110011";
				when "0101"=> H1 <="1011011";
				when "0110"=> H1 <="1011111";
				when "0111"=> H1 <="1110000";
				when "1000"=> H1 <="1111111";
				when "1001"=> H1 <="1111011";
				when "1010"=> H1 <="1110111";
				when "1011"=> H1 <="0011111";
				when "1100"=> H1 <="1001110";
				when "1101"=> H1 <="0111101";
				when "1110"=> H1 <="1001111";
				when "1111"=> H1 <="1000111";
    end case;
    end process;
	
	G_LEDG(3 downto 0) <= O;
	HEX1(6 downto 0) <= NOT(H1);
	end hardware;