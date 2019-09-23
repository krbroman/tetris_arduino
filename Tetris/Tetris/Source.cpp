#include <iostream>
#include <windows.h>
#include <conio.h>
#include <limits>
//������ ���� ������ � �������
int BlocksList[7][4][4]
{
	{
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0}
	},

	{
		{0, 1, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	},
	
	{
		{0, 0, 1, 0},
		{0, 1, 1, 0},
		{0, 1, 0, 0},
		{0, 0, 0, 0}
	},

	{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0}
	},

	{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 0}
	},

	{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0}
	},

	{
		{0, 0, 0, 0},
		{0, 1, 0, 0},
		{1, 1, 1, 0},
		{0, 0, 0, 0}
	}

};

class Tetris
{
	private:
		//�������� ����
		int gameSpeed = 20000;
		//������� ������� ����
		int level = 0;
		//������� ��� ������� ����
		int block[4][4];
		//������� ��� ������� �������, � �� � ����������
		int field[20][13];
		//������� ��� ������� ��������� �������
		int stage[20][13];
		//������� ��������� �����
		int x = 5;
		int y = 0;
		//������� ���������
		bool gameover = false;

	public:
		//����� ������ ����
		void StartGame()
		{
			//��������� ������� ����, ������ �������
			for (int i = 0; i < 20; i++)
			{
				for (int j = 0; j < 13; j++)
				{
					if ((j == 0) || (j == 12) || (i == 19))
					{
						field[i][j] = stage[i][j] = 9;
					}
					else
					{
						field[i][j] = stage[i][j] = 0;
					}
				}
			}
			//������� ������ ����
			CreateNewBlock(rand() % 7);
			//���������� ����
			DisplayBoard();
		}

		//����� ����������� ����
		void DisplayBoard()
		{
			//������� ����
			system("cls");

			//������ ���� ��������� ������ ����
			for (int i = 0; i < 20; i++)
			{
				for (int j = 0; j < 13; j++)
				{
					switch (field[i][j])
					{
					case 0:
						std::cout << " ";
						break;
					case 9:
						std::cout << "|";
						break;
					default:
						std::cout << "#";
						break;
					}
				}
				std::cout << std::endl;
			}

			//��� ���������, �������� ����� ����� ���������� ����
			if (gameover)
			{
				system("cls");
				GameOver();
			}
		}

		//�����, ������������ ��� ��������
		void GameOver()
		{
			//������� ����������
			std::cout << "Game Over" << std::endl;
			std::cout << "Level:" << level << std::endl << std::endl;
			std::cout << "Press Any Key To Try Again" << std::endl;

			//����������� ����, ������ �����
			while (true)
				if (_kbhit())
				{
					//������ �������� ����
					gameover = false;
					LoopGame();
				}
		}

		//�����
		void LoopGame()
		{
			//������� �������, �� �������� ����� ����������, ����� ��������� ��������� ����
			int deltaTime = 0;
			StartGame();

			while (!gameover)
			{
				//���� ���� ����, ���������
				if(_kbhit())
					GetInput();

				//���� ������� �� ������ ������� ��������, �������� ���
				if (deltaTime < gameSpeed)
				{
					deltaTime++;
					
				}
				else
				{
					//��������� ��������� ���� � �������� �������
					GameFlow();
					deltaTime = 0;
				}
			}

			
		}
		//����� �������� ������ �����
		void CreateNewBlock(int blockType)
		{
			//������������� ��������� ��������� ����� �� ����
			x = 4;
			y = 0;

			//��������� ������� ����� ��� ��������� ����
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					block[i][j] = 0;
					block[i][j] = BlocksList[blockType][i][j];
				}
			}

			//��������� ������� ��������� ��������
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[i][j + 4] = stage[i][j + 4] + block[i][j];

					//���� �� ��� �����, ��� �������� ����� ����, ��� ���-�� ����, �� ����� ����
					if (field[i][j + 4] > 1)
					{
						gameover = true;
						
					}
				}
			}
			
		}

		//����� ����������� �����
		void MoveBlock(int sourceX, int sourceY)
		{
			//������� ����
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[y + i][x + j] -= block[i][j];
				}
			}
			//��������� �������
			x = sourceX; y = sourceY;

			//�������������� ����
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[y + i][x + j] += block[i][j];
				}
			}

			//���������� ����� ����
			DisplayBoard();
		}

		//����� ���������� �������� ��������� ����
		void GameFlow()
		{
			//���� ���� �� ������������ � ������ ������, ���������� �� ���� ������ ����
			if (!isColliding(x, y + 1))
			{
				MoveBlock(x, y + 1);
			}
			else
			{
				//���� ������������, ��������� ������
				Collided();
				//���������, ���� �� ������ ���� ��� �� �������
				CheckForFullRows();
				//������� ����� ����
				CreateNewBlock(rand() % 7);
				//���������� ����
				DisplayBoard();
			}
		}

		//����� �������� �� ������� ������ �����
		void CheckForFullRows()
		{
			for (int i = 19; i >= 0; i--)
			{
				int tmp = 0;
				for (int j = 1; j < 12; j++)
					tmp += stage[i][j];

				//���� � ���� ��� ������� (�� ������), �� ��������� ����, ������ ���� ���
				if (tmp == 11)
				{
					Update(i);
					//���������� �������� � �������� ����, �� �� ���������
					i++;
				}
				
			}
		}

		//����� �������� ����
		void Update(int row)
		{
			//��������� �� ����� �������
			level++;
			gameSpeed -= level * level * 100;

			//������� ���
			for (int j = 1; j < 12; j++)
			{
				field[row][j] = stage[row][j] = 0;
				DisplayBoard();
			}

			//���������� ��� ����, ���� ����������, �� ��� ����
			for (int i = row; i > 0; i--)
			{
				for (int j = 1; j < 12; j++)
					field[i][j] = stage[i - 1][j];
			}
			for (int j = 1; j < 12; j++)
				field[0][j] = 0;

			for (int i = 0; i < 20; i++)
			{
				for (int j = 0; j < 13; j++)
					stage[i][j] = field[i][j];
			}
			//���������� ����
			DisplayBoard();
		}

		//�������� �� ��, ������������ �� ���� � ���-���� ��� ����������� ���� ��������
		bool isColliding(int sourceX, int sourceY)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (block[i][j] && stage[sourceY + i][sourceX + j] != 0)
					{
						return true;
					}
				}
			}
			return false;
		}

		//����� ���������� ������ ��� ������� �����
		void Collided()
		{
			for (int i = 0; i < 20; i++)
			{
				for (int j = 0; j < 13; j++)
				{
					stage[i][j] = field[i][j];
				}
			}
		}
		
		//����� ��������� �����
		void GetInput()
		{
			char key;

			key = _getch();//getchar();

			//��� ��������� d � �������, ��� �� �� ������������ � ������� ���������, ������� ������
			// a - �����; s - ����; ������ - �������
			
			switch (key)
			{
			case 'd':
				if (!isColliding(x + 1, y))
				{
					MoveBlock(x + 1, y);
				}
				break;
			case 'a':
				if (!isColliding(x - 1, y))
				{
					MoveBlock(x - 1, y);
				}
				break;
			case 's':
				if (!isColliding(x, y + 1))
				{
					MoveBlock(x, y + 1);
				}
				break;
			case ' ':
				RotateBlock();
			}


		}

		//����� �������� �� ������� �������
		void RotateBlock()
		{
			//���������� ������� ��������� �����
			int tmp[4][4];

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					tmp[i][j] = block[i][j];

			//������������ ����
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					block[i][j] = tmp[j][3 - i];

			//���� ���� ������������, ���������� �������� ��������� � ������� �� ������
			if (isColliding(x, y))
			{
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						block[i][j] = tmp[i][j];

				return;
			}

			//���� ��� � �������, �������������� ���� � ���������� ����
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[y + i][x + j] -= tmp[i][j];
					field[y + i][x + j] += block[i][j];
				}
			}

			DisplayBoard();
		}
};

int main()
{
	Tetris tetris;
	tetris.LoopGame();

	system("pause");
	return 0;
}