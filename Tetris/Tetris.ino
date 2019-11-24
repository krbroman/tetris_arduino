#include <LCD5110_Graph.h>
#include <EEPROM.h>
LCD5110 LCD(9,10,11,12,13);

extern unsigned char SmallFont[];
#define ANALOG_X_pin 0
#define ANALOG_Y_pin 1
#define RIGHT_pin 3
#define LEFT_pin 5
#define UP_pin 2
#define DOWN_pin 4
#define BUTTON_E 6
#define BUTTON_F 7
#define BUTTON_G 8

//Функция сброса
void(* resetFunc) (void) = 0;

//Список всех блоков в тетрисе
uint8_t BlocksList[7][4][4]
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
    int gameSpeed = 600;
    //Текущий уровень игры
    uint8_t level = 0;
    int score = 0;
    //Матрица под текущий блок
    uint8_t block[4][4];
    //Матрица под текущие объекты, в тч и движущиеся
    uint8_t field[20][13];
    //Матрица под текущие статичные объекты
    uint8_t stage[20][13];
    //Текущее положение блока
    uint8_t x = 5;
    uint8_t y = 0;
    //Условие проигрыша
    bool gameover = false;
    //Настройки: размер, контраст, сложность
    uint8_t size = 2;
    uint8_t contrast = 70;
    uint8_t difficulty = 0;
    //Выбранные в данный момент параметры в меню
    uint8_t _chosenOption = 0;
    uint8_t _section = 0;
    //Переменная, которая показывает, играем мы в данный момент или нет
    uint8_t _isPlaying = 0;
    //Таблица рекордов
    String Top1 = "ZZZ";
    uint8_t Top1level = 0;
    int Top1score = 0;

    String Top2 = "ZZZ";
    uint8_t Top2level = 0;
    int Top2score = 0;

    String Top3 = "ZZZ";
    uint8_t Top3level = 0;
    int Top3score = 0;
    
  public:
    
    //Метод, вызывающий меню
    int ShowMenu()
    {
      while(true)
      {
        //Меняем выбранную опцию кнопками вверх и вниз
        if(digitalRead(UP_pin)==LOW)
          if(_chosenOption == 0)
            _chosenOption = 3;
            else _chosenOption -= 1;

        if(digitalRead(DOWN_pin)==LOW)
          if(_chosenOption == 3)
            _chosenOption = 0;
            else _chosenOption += 1;
        LCD.clrScr();
        LCD.drawRect(0,5 + _chosenOption*8, 6,11 + _chosenOption*8);

        //Вывод начальной панели меню
        if(_section == 0)
        {
              LCD.print("START",27,5);
              LCD.print("RECORDS",20,13);
              LCD.print("SETTINGS",17,21);
              LCD.print("EXIT",30,29);
              LCD.update();
              if(digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW||digitalRead(BUTTON_G)==LOW)
                //Действия для кнопки START
                if(_chosenOption == 0)
                {
                  //Начинаем игру
                  LCD.clrScr();
                  _isPlaying = 1;
                  LoopGame();
                }
                //Для кнопки RECORDS
                else if(_chosenOption == 1)
                {
                  //Переходим к таблице рекордов
                  LCD.clrScr();
                  _section = 2;
                  _chosenOption = 0;
                  delay(100);
                }
                //Для кнопки SETTINGS
                else if(_chosenOption == 2)
                {
                  //Переходим к меню настроек
                  LCD.clrScr();
                  _section = 1;
                  _chosenOption = 0;
                  delay(100);
                }
                //Для кнопки EXIT
                else if(_chosenOption == 3)
                {
                  //Выводим прощальное сообщение и выключаем экран
                  LCD.clrScr();
                  LCD.print("Goodbye!",0,0);
                  LCD.update();
                  delay(1000);
                  LCD.clrScr();
                  LCD.update();
                  LCD.enableSleep();
                  _isPlaying = 5;
                }
        }
        //Вывод меню настроек
        else if(_section == 1)
        {
              
              LCD.print("BACK",8,5);
              LCD.print("SIZE",8,13);
              LCD.printNumI(size,76,13);
              LCD.print("DIFFICULTY",8,21);
              LCD.printNumI(difficulty,76,21);
              LCD.print("CONTRAST",8,29);
              LCD.printNumI(contrast,70,29);
              LCD.update();
              if(digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW||digitalRead(BUTTON_G)==LOW)
                if(_chosenOption == 0)
                {
                  //Возвращаемся в изначальное меню
                  LCD.clrScr();
                  _section = 0;
                  _chosenOption = 0;
                  delay(300);
                }
                if(_chosenOption == 1)
                {
                  //Меняем размер от 1 до 3
                  if(digitalRead(RIGHT_pin)==LOW)
                  if(size == 3)
                    size = 1;
                  else size++;
                else if(digitalRead(LEFT_pin)==LOW)
                  if(size == 1)
                    size = 3;
                  else size--;
                  EEPROM.put(0, size);
                }
                else if(_chosenOption == 2)
                {
                  //Меняем сложность(скорость) от 0 до 3
                  if(digitalRead(RIGHT_pin)==LOW)
                  if(difficulty == 3)
                    difficulty = 0;
                  else difficulty++;
                else if(digitalRead(LEFT_pin)==LOW)
                  if(difficulty == 0)
                    difficulty = 3;
                  else difficulty--;
                  EEPROM.put(9, difficulty);
                }
                else if(_chosenOption == 3)
                {
                  //Меняем контраст
                  if(digitalRead(RIGHT_pin)==LOW)
                  if(contrast == 70)
                    contrast = 50;
                    else contrast += 2;
                else if(digitalRead(LEFT_pin)==LOW)
                  if(contrast == 50)
                    contrast = 70;
                    else contrast -= 2;
                    LCD.setContrast(contrast);

                   EEPROM.put(18, contrast);
                }
        }
        //Вывод таблицы рекордов
        else if (_section == 2)
        {
          LCD.print("BACK",8,5);
          LCD.print("1]",8,13);
          LCD.print(Top1,20,13);
          LCD.printNumI(Top1level,40,13);
          LCD.printNumI(Top1score,47,13);

          LCD.print("2]",8,21);
          LCD.print(Top2,20,21);
          LCD.printNumI(Top2level,40,21);
          LCD.printNumI(Top2score,47,21);

          LCD.print("3]",8,29);
          LCD.print(Top3,20,29);
          LCD.printNumI(Top3level,40,29);
          LCD.printNumI(Top3score,47,29);

          LCD.update();
          if(digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW||digitalRead(BUTTON_G)==LOW)
                if(_chosenOption == 0)
                {
                  LCD.clrScr();
                  _section = 0;
                  _chosenOption = 0;
                  delay(300);
                }
        }
        delay(100);        
      }
    }
    //Метод отрисовки поля с размером size
    //По идее, можно выбрать любой size, но при size > 2 поле не помещается на LCD5110
    void Print(int i, int j)
    {
      
       for (int _i = 0; _i < size; ++_i)
          for (int _j = 0; _j < size; ++_j)
              LCD.setPixel(size*j+_j,size*i+_i);
    }
    
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
      LCD.clrScr();

      //Рисуем поле пикселями вместо цифр
      for (int i = 0; i < 20; i++)
      {
        for (int j = 0; j < 13; j++)
        {
          switch (field[i][j])
          {
          case 0:
            
            break;
          case 9:
            
            Print(i,j);
            break;
          default:
            
            Print(i,j);
            break;
          }
        }
        
      }
      
      //Выводим очки
      LCD.print("Score:",13*size,size*2);
      LCD.printNumI(score,13*size + 35,size*2);

      //При проигрыше, вызываем метод метод завершения игры
      if (gameover)
      {
        LCD.clrScr();
        
        GameOver();
      }
      LCD.update();
    }

    //Метод, вызывающийся при прогрыше
    void GameOver()
    {
      //Выводим информацию: уровень и очки
      LCD.clrScr();
      LCD.print("Game Over",0,0);
      LCD.print("Level:",0,8);
      LCD.printNumI(level,40,8);
      
      LCD.print("Score:",0,16);
      LCD.printNumI(score,40,16);
      LCD.print("Press Any Key",0,24);
      LCD.print("To Continue",0,32);
      LCD.update();
      //Бесконечный цикл, ждущий ввода
      while (true)
        if (digitalRead(UP_pin)==LOW||digitalRead(DOWN_pin)==LOW||digitalRead(LEFT_pin)==LOW||digitalRead(RIGHT_pin)==LOW||digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW||digitalRead(BUTTON_G)==LOW)
        {
          String _name = "AAA";
          uint8_t _chosenChar = 0;
          while (true)
          {
            //Ввод имени игрока
            if(digitalRead(LEFT_pin)==LOW)
              if(_chosenChar == 0)
                _chosenChar = 2;
                else _chosenChar -= 1;

            if(digitalRead(RIGHT_pin)==LOW)
              if(_chosenChar == 2)
                _chosenChar = 0;
                else _chosenChar += 1;
                
            if(digitalRead(DOWN_pin)==LOW)
              if(_name[_chosenChar] == 'A')
                _name[_chosenChar] = 'Z';
                else _name[_chosenChar] -= 1;

            if(digitalRead(UP_pin)==LOW)
              if(_name[_chosenChar] == 'Z')
                _name[_chosenChar] = 'A';
                else _name[_chosenChar] += 1;
 
            LCD.clrScr();
            LCD.print("Enter Name:",0,0);
            LCD.print(_name,64,0);
            LCD.drawRect(64 + _chosenChar*6,8,70 + _chosenChar*6,10);

            LCD.print("Press G",0,12);
            LCD.print("To Try Again",0,20);
            LCD.print("Press E,F",0,28); 
            LCD.print("To Menu",0,36);

            

            LCD.update();
            delay(200);
            //Далее просчитываем, надо ли менять позиции в таблице рекордов и по кнопке G начинаем игру заново
            //А по кнопкам E и F переходим в меню

            if (digitalRead(BUTTON_G)==LOW)
            {
              
              delay(1000);
              CalcRecords(_name, level, score);
              
              gameover = false;
              LCD.clrScr();
              LoopGame();
            }
            if (digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW)
            {
              LCD.clrScr();
              CalcRecords(_name, level, score);
              _isPlaying = 0;
              gameover = false;
              //return;
              LCD.update();
              delay(1000);
              resetFunc();
              
              
            }
          }
        }
    }

    //Метод пересчета позиций в таблице рекордов
    void CalcRecords(String _name, int _level, int _score)
    {
      
      if (_score >= Top1score)
      {
        if (_score == Top1score)
        {
          if (_name.compareTo(Top1) < 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = Top1[i];
            Top2score = Top1score;
            Top2level = Top1level;
            
            for (int i = 0; i<3;++i)
              Top1[i] = _name[i];
            Top1score = _score;
            Top1level = _level;
          }
          else if(_name.compareTo(Top1) > 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
          }
          else if (_name.compareTo(Top1) == 0)
          {
            for (int i = 0; i<3;++i)
              Top1[i] = _name[i];
            Top1score = _score;
            Top1level = _level;
          }
        }
        else
        {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = Top1[i];
            Top2score = Top1score;
            Top2level = Top1level;
            
            for (int i = 0; i<3;++i)
              Top1[i] = _name[i];
            Top1score = _score;
            Top1level = _level;
        }
      }
      else if (_score >= Top2score)
      {
        if (_score == Top2score)
        {
          if(_name.compareTo(Top2) < 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
          }
          else if(_name.compareTo(Top2) > 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = _name[i];
            Top3score = _score;
            Top3level = _level;
          }
          else if(_name.compareTo(Top2) == 0)
          {
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
          }
        }
        else
        {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
        }
      }
      else if (_score >= Top3score)
      {
            for (int i = 0; i<3;++i)
              Top3[i] = _name[i];
            Top3score = _score;
            Top3level = _level;
      }

      //Запоминаем топ3 в памяти
      for (int i = 0; i < 3; ++i)
        EEPROM.write(237 + i*9, Top1[i]);
      for (int i = 0; i < 3; ++i)
        EEPROM.write(362 + i*9, Top2[i]);
      for (int i = 0; i < 3; ++i)
        EEPROM.write(487 + i*9, Top3[i]);

      EEPROM.put(111, Top1score);
      EEPROM.put(128, Top2score);
      EEPROM.put(145, Top3score);
      EEPROM.put(162, Top1level);
      EEPROM.put(171, Top2level);
      EEPROM.put(180, Top3level);
    }

    
    void LoopGame()
    {
      //Выставляем настройки и начинаем игру
      LCD.setContrast(contrast);
      gameSpeed = 600;
      gameSpeed -= difficulty * 100;
      level = 0;
      score = 0;
      StartGame();

      bool f = false;
      while (!gameover)
      {
        //Считываем ввод
        f = !f;
          GetInput();

        //Обновляем состояние игры
        if(f)
         GameFlow();
        delay(gameSpeed);
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
      //Обновляем позицию
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
      int rows = 0;
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
          rows++;
        }
        
      }
      //Начисляем очки, в зависимости от количества убранных за раз рядов
      if(rows == 0)
        return;
      if (rows == 1)
        score += 100;
        else if (rows == 2)
        score += 300;
        else if (rows == 3)
        score += 700;
        else if (rows == 4)
        score += 1500;
    }


    //Метод удаления ряда
    void Update(int row)
    {
      //Переходим на новый уровень
      level++;
      gameSpeed -= level * level;

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
    
    //Метод получение ввода
    void GetInput()
    {
     


      //Перемещение вправо и влево: кнопки B D, движение джойстика вправо и влево
      //Вниз:C, движение джойстика вниз
      //Поворот: A, движение джойстика вверх
      if (digitalRead(UP_pin)==LOW||(analogRead(ANALOG_Y_pin)> 553))
      {
        RotateBlock();
      } 
      else if (digitalRead(DOWN_pin)==LOW||(analogRead(ANALOG_Y_pin)< 493))
      {
        if (!isColliding(x, y + 1))
        {
          MoveBlock(x, y + 1);
        }
      } 
      
      if (digitalRead(RIGHT_pin)==LOW||(analogRead(ANALOG_X_pin)> 400))
      {
        if (!isColliding(x + 1, y))
        {
          MoveBlock(x + 1, y);
        }
      } 
      else if (digitalRead(LEFT_pin)==LOW||(analogRead(ANALOG_X_pin)< 320))
      {
        if (!isColliding(x - 1, y))
        {
          MoveBlock(x - 1, y);
        }
      } 
    }
    //Метод получения параметра _isPlaying
    int GetPlay()
    {
      return _isPlaying;
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

      //Если есть столкновение, возвращаем исходное положение и выходим из метод
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
    //Метод ввода настроек из памяти
    void SetVals()
    {
      size = 2;
      difficulty = 0;
      contrast = 70;

      Top1 = "ZZZ";
      Top1level = 0;
      Top1score = 0;

      Top2 = "ZZZ";
      Top2level = 0;
      Top2score = 0;

      Top3 = "ZZZ";
      Top3level = 0;
      Top3score = 0;
      
      if (EEPROM.read(0) >= 1 && EEPROM.read(0) <= 3)
        EEPROM.get(0,size);
      if (EEPROM.read(9) >= 0 && EEPROM.read(9) <= 3)
        EEPROM.get(9,difficulty);
      if (EEPROM.read(18) >= 50 && EEPROM.read(18) <= 70 && EEPROM.read(18) % 2 == 0)
        EEPROM.get(18,contrast);

        int flag = 0;
        for (int i = 0; i < 3; ++i)
          if(EEPROM.read(237 + i*9) >= 'A' && EEPROM.read(237 + i*9) <= 'Z')
            flag++;
            
        for (int i = 0; i < 3; ++i)
        if(flag == 3)
          EEPROM.get(237 + i*9, Top1[i]);

        flag = 0;
        for (int i = 0; i < 3; ++i)
          if(EEPROM.read(362 + i*9) >= 'A' && EEPROM.read(362 + i*9) <= 'Z')
            flag++;
          
        for (int i = 0; i < 3; ++i)
        if(flag == 3)
          EEPROM.get(362 + i*9, Top2[i]);

        flag = 0;
        for (int i = 0; i < 3; ++i)
          if(EEPROM.read(487 + i*9) >= 'A' && EEPROM.read(487 + i*9) <= 'Z')
            flag++;
          
        for (int i = 0; i < 3; ++i)
        if(flag == 3)
          EEPROM.get(487 + i*9, Top3[i]);
        
      if(EEPROM.read(111) >= 0 && EEPROM.read(111) <= 32000)
        EEPROM.get(111, Top1score);
      if(EEPROM.read(128) >= 0 && EEPROM.read(128) <= 32000)
        EEPROM.get(128, Top2score);
      if(EEPROM.read(145) >= 0 && EEPROM.read(145) <= 32000)
        EEPROM.get(145, Top3score);

      if(EEPROM.read(162) >= 0 && EEPROM.read(162) <= 127)
        EEPROM.get(162, Top1level);
      if(EEPROM.read(171) >= 0 && EEPROM.read(171) <= 127)
        EEPROM.get(171, Top2level);
      if(EEPROM.read(180) >= 0 && EEPROM.read(180) <= 127)
        EEPROM.get(180, Top3level);
    }
};

extern unsigned char SmallFont[];
Tetris tetris;

void setup() {
  
  pinMode(UP_pin, INPUT_PULLUP);
  pinMode(DOWN_pin, INPUT_PULLUP);
  pinMode(RIGHT_pin, INPUT_PULLUP);
  pinMode(LEFT_pin, INPUT_PULLUP);
  pinMode(BUTTON_E, INPUT_PULLUP);
  pinMode(BUTTON_F, INPUT_PULLUP);
  pinMode(BUTTON_G, INPUT_PULLUP);
  
      
  LCD.InitLCD();
  LCD.setFont(SmallFont);
  randomSeed(analogRead(0));
  tetris.SetVals();
  tetris.ShowMenu();

  
}

void loop() {

  LCD.clrScr();
  tetris.ShowMenu();
}
