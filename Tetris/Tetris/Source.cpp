#include <iostream>
#include <windows.h>
#include <conio.h>
#include <limits>
//Список всех блоков в тетрисе
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
		//Скорость игры
		int gameSpeed = 20000;
		//Текущий уровень игры
		int level = 0;
		//Матрица под текущий блок
		int block[4][4];
		//Матрица под текущие объекты, в тч и движущиеся
		int field[20][13];
		//Матрица под текущие статичные объекты
		int stage[20][13];
		//Текущее положение блока
		int x = 5;
		int y = 0;
		//Условие проигрыша
		bool gameover = false;

	public:
		//Метод старта игры
		void StartGame()
		{
			//Заполняем матрицы поля, рисуем границы
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
			//Создаем первый блок
			CreateNewBlock(rand() % 7);
			//Отображаем поле
			DisplayBoard();
		}

		//Метод отображения поля
		void DisplayBoard()
		{
			//Очищаем поле
			system("cls");

			//Рисуем поле символами вместо цифр
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

			//При проигрыше, вызываем метод метод завершения игры
			if (gameover)
			{
				system("cls");
				GameOver();
			}
		}

		//Метод, вызывающийся при прогрыше
		void GameOver()
		{
			//Выводим информацию
			std::cout << "Game Over" << std::endl;
			std::cout << "Level:" << level << std::endl << std::endl;
			std::cout << "Press Any Key To Try Again" << std::endl;

			//Бесконечный цикл, ждущий ввода
			while (true)
				if (_kbhit())
				{
					//Заново стартуем игру
					gameover = false;
					LoopGame();
				}
		}

		//Метод
		void LoopGame()
		{
			//Создаем счетчик, по которому можно определить, когда обновлять состояние игры
			int deltaTime = 0;
			StartGame();

			while (!gameover)
			{
				//Если есть ввод, считываем
				if(_kbhit())
					GetInput();

				//Пока счетчик не достиг нужного значения, повышаем его
				if (deltaTime < gameSpeed)
				{
					deltaTime++;
					
				}
				else
				{
					//Обновляем состояние игры и обнуляем счетчик
					GameFlow();
					deltaTime = 0;
				}
			}

			
		}
		//Метод создания нового блока
		void CreateNewBlock(int blockType)
		{
			//Устанавливаем начальное положение блока на поле
			x = 4;
			y = 0;

			//Обновляем матрицу блока под очередной блок
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					block[i][j] = 0;
					block[i][j] = BlocksList[blockType][i][j];
				}
			}

			//Обновляем матрицу подвижных объектов
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[i][j + 4] = stage[i][j + 4] + block[i][j];

					//Если на том месте, где появился новый блок, уже что-то было, то конец игры
					if (field[i][j + 4] > 1)
					{
						gameover = true;
						
					}
				}
			}
			
		}

		//Метод перемещения блока
		void MoveBlock(int sourceX, int sourceY)
		{
			//Стираем блок
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[y + i][x + j] -= block[i][j];
				}
			}
			//обновляем позицию
			x = sourceX; y = sourceY;

			//Перерисовываем блок
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					field[y + i][x + j] += block[i][j];
				}
			}

			//Отображаем новое поле
			DisplayBoard();
		}

		//Метод обновления текущего состояния игры
		void GameFlow()
		{
			//Если блок не сталкивается с другим блоком, перемещаем на одну клетку вниз
			if (!isColliding(x, y + 1))
			{
				MoveBlock(x, y + 1);
			}
			else
			{
				//Если сталкивается, обновляем марицы
				Collided();
				//Проверяем, есть ли полные ряды для их очистки
				CheckForFullRows();
				//Создаем новый блок
				CreateNewBlock(rand() % 7);
				//Отображаем поле
				DisplayBoard();
			}
		}

		//Метод проверки на наличие полных рядов
		void CheckForFullRows()
		{
			for (int i = 19; i >= 0; i--)
			{
				int tmp = 0;
				for (int j = 1; j < 12; j++)
					tmp += stage[i][j];

				//Если в ряду все единицы (он полный), то обновляем игру, удаляя этот ряд
				if (tmp == 11)
				{
					Update(i);
					//Продолжаем проверку с текущего ряда, тк он обновился
					i++;
				}
				
			}
		}

		//Метод удаления ряда
		void Update(int row)
		{
			//Переходим на новый уровень
			level++;
			gameSpeed -= level * level * 100;

			//Очищаем ряд
			for (int j = 1; j < 12; j++)
			{
				field[row][j] = stage[row][j] = 0;
				DisplayBoard();
			}

			//Перемещаем все ряды, выше удаленного, на ряд ниже
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
			//Отображаем поле
			DisplayBoard();
		}

		//Проверка на то, сталкивается ли блок с чем-либо при перемещении либо повороте
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

		//Метод обновления матриц при падении блока
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
		
		//метод получение ввода
		void GetInput()
		{
			char key;

			key = _getch();//getchar();

			//При получении d и условии, что он не сталкивается с другими объектами, двигаем вправо
			// a - влево; s - вниз; пробел - поворот
			
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

		//Метод поворота по часовой стрелке
		void RotateBlock()
		{
			//Запоминаем текущее положение блока
			int tmp[4][4];

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					tmp[i][j] = block[i][j];

			//Поворачиваем блок
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					block[i][j] = tmp[j][3 - i];

			//Если есть столкновение, возвращаем исходное положение и выходим из метода
			if (isColliding(x, y))
			{
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						block[i][j] = tmp[i][j];

				return;
			}

			//Если все в порядке, перерисовываем блок и отображаем поле
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