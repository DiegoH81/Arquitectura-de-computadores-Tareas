#include <iostream>
#include <bitset>
#include <cmath>

class FloatPoint
{
private:
	std::bitset <1> sign_1, sign_2, sign_final, expected_sign;
	std::bitset <8> biased_exponent_1, biased_exponent_2, biased_exponent_final, expected_biased_exponent;
	std::bitset <23> significand_1, significand_2, significand_final, expected_significand;

	//FUNCIONES
	std::bitset <8> get_int_bits(int in_num)
	{
		std::string temp_str;
		int aux_num;
		std::bitset <8> temp_int_part;
		do
		{
			aux_num = in_num % 2;
			in_num = in_num / 2;
			temp_str += static_cast<char> (aux_num + 48);
		} while (in_num > 0);

		if (temp_str.length() <= 8)
		{
			for (int index = 0; index < temp_str.length(); index++)
			{
				temp_int_part[index] = static_cast<int> (temp_str[index] - 48);
			}
		}
		return temp_int_part;
	}
	std::bitset <16> get_int_bits_for_expected_number(int in_num)
	{
		std::string temp_str;
		int aux_num;
		std::bitset <16> temp_int_part;
		do
		{
			aux_num = in_num % 2;
			in_num = in_num / 2;
			temp_str += static_cast<char> (aux_num + 48);
		} while (in_num > 0);


		for (int index = 0; index < temp_str.length(); index++)
		{
			temp_int_part[index] = static_cast<int> (temp_str[index] - 48);
		}
		return temp_int_part;
	}
	std::bitset <32> get_floating_bits(float in_num)
	{
		std::bitset <32> temp_float_part;
		for (int index = 31; index >= 0; index--)
		{
			in_num = in_num * 2;
			if (in_num >= 1)
			{
				temp_float_part[index] = 1;
				in_num--;
			}
			else
				temp_float_part[index] = 0;
		}
		return temp_float_part;
	}
	std::bitset <23> normalize_numbers(std::bitset<8> temp_int_bits, std::bitset <32> temp_floating_bits, int exponent)
	{
		std::bitset <23> temp;
		int temp_index{ 22 };
		for (int index = exponent - 1; index >= 0; index--, temp_index--)
		{
			temp[temp_index] = temp_int_bits[index];
		}
		for (int index = 31; temp_index >= 0; index--, temp_index--)
		{
			temp[temp_index] = temp_floating_bits[index];
		}
		return temp;
	}

	std::bitset <23> normalize_numbers(std::bitset<16> temp_int_bits, std::bitset <32> temp_floating_bits, int exponent)
	{
		std::bitset <23> temp;
		int temp_index{ 22 };
		for (int index = exponent - 1; index >= 0; index--, temp_index--)
		{
			temp[temp_index] = temp_int_bits[index];
		}
		for (int index = 31; temp_index >= 0; index--, temp_index--)
		{
			temp[temp_index] = temp_floating_bits[index];
		}
		return temp;
	}
	std::bitset <23> normalize_numbers(std::bitset <46> in_bits, int exponent)
	{
		std::bitset <23> temp;
		int temp_index{ 22 };
		for (int index = exponent - 1; temp_index >= 0; index--, temp_index--)
		{
			temp[temp_index] = in_bits[index];
		}
		return temp;
	}

	std::bitset <8> get_biased(int exponente)
	{
		return get_int_bits(exponente + 127);
	}
	std::bitset <8> adder(std::bitset<8> in1, std::bitset<8> in2)
	{
		int carry{ 0 };
		std::bitset <8> temp;
		for (int index = 0; index < 8; index++)
		{
			temp[index] = (in1[index] ^ in2[index]) ^ carry;
			carry = (in1[index] & (in2[index] | carry)) | (in2[index] & carry);
		}

		return temp;
	}
	std::bitset <23> adder(std::bitset<23> in1, std::bitset<23> in2, int &carry)
	{
		carry = 0;
		std::bitset <23> temp;
		for (int index = 0; index < 23; index++)
		{
			temp[index] = (in1[index] ^ in2[index]) ^ carry;
			carry = (in1[index] & (in2[index] | carry)) | (in2[index] & carry);
		}

		return temp;
	}
	void move_right(std::bitset <23>& A, std::bitset <23>& Q, int carry)
	{
		int temp_num{ A[0] };
		for (int index = 0; index < 22; index++)
			A[index] = A[index + 1];
		A[22] = carry;
		for (int index = 0; index < 22; index++)
			Q[index] = Q[index + 1];
		Q[22] = temp_num;
	}
	std::bitset <46> multiply(std::bitset <23> in_M, std::bitset <23> in_Q)
	{
		int c{ 0 };
		std::bitset <23> A, M, Q;


		M[22] = 1;
		Q[22] = 1;

		for (int index = 21; index >= 0; index--)
		{
			M[index] = in_M[index + 1];
			Q[index] = in_Q[index + 1];
		}
		std::bitset <46> temp;
		for (int index = 0; index < Q.size(); index++)
		{
			if (Q[0] == 1)
				A = adder(A, M, c);
			move_right(A, Q, c);
			if (c == 1)
				c = 0;
		}

		for (int index = 0; index < Q.size(); index++)
		{
			temp[index] = Q[index];
			temp[index + 23] = A[index];
		}
		return temp;
	}

	int get_exponent(std::bitset<8> bits)
	{
		int pos{ -1 };
		for (int index = 7; index >= 0; index--)
		{
			if (bits[index] == 1)
			{
				pos = index;
				break;
			}
		}
		return pos;
	}
	int get_exponent(std::bitset<16> bits)
	{
		int pos{ -1 };
		for (int index = 15; index >= 0; index--)
		{
			if (bits[index] == 1)
			{
				pos = index;
				break;
			}
		}
		return pos;
	}

	int get_exponent(std::bitset<46> bits) //MULTIPLICANDO
	{
		if (bits[45] == 1)
			return 1;
		else
			return 0;
	}
	int get_limit (std::bitset<46> bits) //MULTIPLICANDO
	{
		int pos{ -1 };
		for (int index = 45; index >= 0; index--)
		{
			if (bits[index] == 1)
			{
				pos = index;
				break;
			}
		}
		return pos;
	}

	float get_floating_part(float in_float)
	{
		do
		{
			if ((in_float - 1) < 0)
				return in_float;
			else
				in_float--;
		} while (true);
	}
	int from_bit_to_int(std::bitset <8> in)
	{
		int temp{ 0 };
		for (int index = 0; index < 8; index++)
			temp += pow(2, index) * in[index];
		return temp;
	}

	void binary_convertion(float in_num, std::bitset <1>& in_sign, std::bitset <8>& in_biased_exponent, std::bitset <23>& in_significand)
	{
		int temp_num_int{ -1 }, exponent{ -1 };
		std::bitset <8>  temp_int_bits;
		std::bitset <32> temp_floating_bits;
		temp_num_int = in_num;
		if (in_num != 0)
		{
			if (in_num > 0)
				in_sign = 0;
			else
			{
				in_sign = 1;
				in_num = in_num * -1;
				temp_num_int = in_num;
			}
			temp_floating_bits = get_floating_bits(get_floating_part(in_num));
			

			if (in_num > 255)
			{
				std::bitset <16> temp_int_bits_16{ get_int_bits_for_expected_number(temp_num_int) };
				exponent = get_exponent(temp_int_bits_16);
				in_significand = normalize_numbers(temp_int_bits_16, temp_floating_bits, exponent);
			}
			else
			{
				temp_int_bits = get_int_bits(temp_num_int);
				exponent = get_exponent(temp_int_bits);
				in_significand = normalize_numbers(temp_int_bits, temp_floating_bits, exponent);
			}
			in_biased_exponent = get_biased(exponent);
		}
	}
public:
	FloatPoint()
	{}

	void algorithm(float in_num1, float in_num2)
	{
		binary_convertion(in_num1, sign_1, biased_exponent_1, significand_1);
		binary_convertion(in_num2, sign_2, biased_exponent_2, significand_2);
		binary_convertion(in_num1 * in_num2, expected_sign, expected_biased_exponent, expected_significand);
		if ((in_num1 != 0) && (in_num2 != 0))
		{
			std::bitset<8> temp_exponent{ adder(adder(biased_exponent_1, 0b10000001), biased_exponent_2) };
			int temp_exponent_int{ from_bit_to_int(temp_exponent) },
				temp_biased_1{ from_bit_to_int(biased_exponent_1) },
				temp_biased_2{ from_bit_to_int(biased_exponent_2) };
			if ((temp_exponent_int < (temp_biased_2 + temp_biased_1 - 127)))
			{
				// Overflow, NO HABRIA underflow ya que el exponente nunca podria ser negativo
				std::cout << "Overflow.\n";
				return;
			}
				
			std::bitset<46> temp_significand{ multiply(significand_1, significand_2) };
			int temp_exponent_limit{ get_limit(temp_significand) };

			if (in_num1 < 0)
				sign_final = 1;
			else if (in_num2 < 0)
				sign_final = 1;
			if ((in_num1 < 0) && (in_num2 < 0))
				sign_final = 0;


			biased_exponent_final = adder(temp_exponent, get_int_bits(get_exponent(temp_significand)));
			significand_final = normalize_numbers(temp_significand, temp_exponent_limit);	
		}
	}

	void menu()
	{
		float input_1{ 0 }, input_2{ 0 };
		do
		{
			std::cout << "Ingrese el primer numero: ";
			std::cin >> input_1;
			std::cout << "Ingrese el segundo numero: ";
			std::cin >> input_2;
			if ((input_1 > 255 || input_2 > 255) || ((input_1 < -255 || input_2 < -255)))
				std::cout << "Numeros invalidos!\n";
		} while ((input_1 > 255 || input_2 > 255) || ((input_1 < -255 || input_2 < -255)));
		algorithm(input_1, input_2);

		std::cout << "Primer numero: " << input_1 << "\n";
		std::cout << sign_1 << " - " << biased_exponent_1 << " - " << significand_1 << "\n\n";
		std::cout << "Segundo numero: " << input_2 << "\n";
		std::cout << sign_2 << " - " << biased_exponent_2 << " - " << significand_2 << "\n\n";
		std::cout << "--------------------------------------------------" << "\n";
		std::cout << "Resultado esperado: " << input_1 * input_2 << "\n";
		std::cout << expected_sign << " - " << expected_biased_exponent << " - " << expected_significand << "\n\n";
		std::cout << "Resultado de la multiplicacion: " << "\n";
		std::cout << sign_final << " - " << biased_exponent_final << " - " << significand_final << "\n";
	}
};

int main()
{
	FloatPoint test;
	test.menu();
}
