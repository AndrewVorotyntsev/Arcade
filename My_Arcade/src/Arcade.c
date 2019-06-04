/*
 ============================================================================
 Name        : My_Animation.c
 Author      : AndrewVorotyntsev
 Version     :
 Copyright   : Your copyright notice
 Description : Перемещение персонажем(циклопом) на стрелки.
 При нажатии на цифры 1 , 2 , 3 появляются соответствующее количество листьев.
 Нажатия на клавиши Q и E уменьшают (или увеличивают соответственно) количество
 ледяных элементалей.
 Данные(о кол-ве вызванных объектов) хранятся в файле Save.txt . Они загружаются оттуда
 при запуске программы , и сохраняются туда при завершении игры.
 Все ресурсы (изображения объектов) хранятся в папке resources.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#if defined(__MINGW32__)
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

// Размеры окна для вывода
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// путь к файлу с картинкой, содержащей файлы движения
char portal[] = "resources/greenPortal.png";
char sprite[] = "resources/cyclops.png";
char mob[] = "resources/ice.png";
char leaf[] = "resources/leaf.png";
//Перемещение по осям(длинна шага)
int posx = 50;
int posy = 50;

// Функция для инициализации библиотеки и создания окна
int initSDL();
// функция для проведения завершения работы библиотеки и освобождения всех ресурсов
void closeSDL();
// загрузка изображения
SDL_Texture* loadImage(char imagePath[]);


// Указатель на структуру, описывающую окно для вывода графики
SDL_Window* window = NULL;
// Указательн на структуру, описывающую механизм отрисовки в окне с применением аппаратного ускорения
SDL_Renderer* renderer = NULL;
SDL_Renderer* showmob = NULL;
// указатель на поверхность (surface)
SDL_Surface* screenSurface = NULL;

// Переенные для хранения анимации персонажа
SDL_Texture* sprite_sheet;
SDL_Texture* mob_sheet;
SDL_Texture* portal_sheet;
SDL_Texture* leaf_sheet;
// текущий кадр анимации
int anim_phase_c = 0;
int anim_phase_p = 0;
int anim_phase_m = 0;
int anim_phase_l = 0;
// фаза анимации (бег, стояние на месте ...)
int anim_type = 0;
// количество кадров анимации для данного типа
int anim_phase_max[20] = { 15,12,7,13,3,5,9,4,6,8,14,12,7,13,3,5,9,4,6,8};

// время с предыдущего кадра анимации
uint32_t last_frame;
// время в мс на 1 кадр
#define frame_time  200


int main(int argc, char *argv[]) {
    //Загружаем сохраненные данные
    int l,m;
    FILE *save ;
    save = fopen("Save.txt", "r");
    fscanf(save ,"%i" , &l);
    fscanf(save ,"%i" , &m);
    fclose(save);
    save = fopen("Save.txt", "w");
    // Инициализируем библиотеку SDL
    if (initSDL() > 1) {
        printf("Error in initialization.\n");
    } else {
        //Работа со списком


        // Загружаем картирнку из файла
        sprite_sheet = loadImage(sprite);
        mob_sheet = loadImage(mob);
        portal_sheet = loadImage(portal);
        leaf_sheet = loadImage(leaf);
        SDL_Rect obj_size, screen_size;
        last_frame=SDL_GetTicks();

        int quit = 0;
        // Структура для хранения информации о событии
        SDL_Event event;
        // Основной цикл программы, выход из которого происходит при появлении события выхода (от ОС или по нажатию ESC)
        // появлении события выхода (от ОС или по нажатию ESC)
        while (quit == 0)
        {
            // Ждём появления события
            while (SDL_PollEvent(&event) != 0)
            {
                // При его появлении проверяем тип
                if (event.type == SDL_QUIT)
                    // Если событие выхода (сигнал от ОС, что приложение
                    // нужно завершить), меняем флаг выхода
                    quit = 1;
                else if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_UP:
                        anim_type = 5;
                        posy=posy-10;
                        break;
                    case SDLK_DOWN:
                        anim_type = 2;
                        posy=posy+10;
                        break;
                    case SDLK_LEFT:
                        anim_type = 11;
                        posx=posx-10;//scale --;
                        break;
                    case SDLK_RIGHT:
                        anim_type = 1;
                        posx=posx+10;
                        break;
                    case SDLK_1:
                        l = 1;
                        break;
                    case SDLK_2:
                        l = 2;
                        break;
                    case SDLK_3:
                        l = 3;
                        break;
                    case SDLK_e:
                        m = m + 1;
                        break;
                    case SDLK_q:
                        m = m - 1;
                        break;
                    case SDLK_TAB:
                        anim_type++;
                        break;
                    case SDLK_ESCAPE:
                        // Нажата клавиша ESC, меняем флаг выхода
                        quit = 1;
                        break;
                    }
                }
            }
            // отрисовка картинки с новым кадром анимации
            // проверка прошедшего времени:
            if ((SDL_GetTicks()- last_frame) >= frame_time) {
                anim_phase_c++;
                anim_phase_p++;
                anim_phase_m++;
                anim_phase_l++;
                if (anim_phase_c >= anim_phase_max[anim_type]) anim_phase_c = 0;
                last_frame = SDL_GetTicks();
                if (anim_phase_p >= 8) anim_phase_p = 0;
                last_frame = SDL_GetTicks();
                if (anim_phase_m >= 6) anim_phase_m = 0;
                last_frame = SDL_GetTicks();
                if (anim_phase_l >= 5) anim_phase_l = 0;
                last_frame = SDL_GetTicks();
            }
            // часть картинки с нужным кадром анимации
            obj_size.x = 64 * anim_phase_c;
            obj_size.y = 64 * anim_type;
            obj_size.h = 64;
            obj_size.w = 64;
            // место для вывода кадра анимации и его увеличение
            screen_size.x = 64+posx;
            screen_size.y = 64+posy;
            screen_size.h = 64 * 4;
            screen_size.w = 64 * 4;
            // Очищаем буфер рисования
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, sprite_sheet, &obj_size, &screen_size);
            //Цикл вывода ледяных элементалей
            for (int z=0;z<m;z++)
            {
                obj_size.x = 32 * anim_phase_m;
                obj_size.y = 0;
                obj_size.h = 32;
                obj_size.w = 32;
                screen_size.x = 76 + z*50;
                screen_size.y = 64;
                screen_size.h = 128;
                screen_size.w = 128;
                SDL_RenderCopy(renderer, mob_sheet, &obj_size, &screen_size);
            }
            //Вывод портала
            obj_size.x = 64 * anim_phase_p;
            obj_size.y = 0;
            obj_size.h = 64;
            obj_size.w = 64;
            screen_size.x = 0;
            screen_size.y = 32;
            screen_size.h = 192;
            screen_size.w = 192;
            SDL_RenderCopy(renderer, portal_sheet, &obj_size, &screen_size);
            //Цикл вывода листьев
            for (int y=0;y<l;y++)
            {
                obj_size.x = 32 * anim_phase_l;
                obj_size.y = 32;
                obj_size.h = 32;
                obj_size.w = 32;
                screen_size.x = 64 + y*50;
                screen_size.y = 84 + y*50;
                screen_size.h = 192;
                screen_size.w = 192;
                SDL_RenderCopy(renderer, leaf_sheet, &obj_size, &screen_size);
            }
            // Выводим буфер на экран
            SDL_RenderPresent(renderer);

        }
        //Сохраняем данные
        fprintf(save,"%i %i",l,m);
    }
    closeSDL();
    return EXIT_SUCCESS;
}



int initSDL() {
    int success = 1;
    // Инициализируем библиотеку SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // при появлении ошибки выводим её описание
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = 0;
    } else {
        // После успешной инициализации создаём окно для вывода графики
        window = SDL_CreateWindow("SDL example", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            // выводим ошибку, если создать окно не удалось
            printf("Window could not be created! SDL_Error: %s\n",
                    SDL_GetError());
            success = 0;
        } else {
            // Инициализируем библиотеку загрузки изображений
            int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
                printf("SDL_image could not initialize! SDL_image Error: %s\n",
                IMG_GetError());
                success = 0;
            }
            // Получаем поверхность для рисования
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                // выводим ошибку, если создать окно не удалось
                printf("Renderer could not be created! SDL_Error: %s\n",
                        SDL_GetError());
                success = 0;
            } else {
                // Задаём цвет отрисовки по умолчанию - белый
                SDL_SetRenderDrawColor(renderer, 0xF, 0xF, 0xFF, 0xF);
            }
        }
    }
    return success;
}

void closeSDL() {
    // Закрываем окно и освобождаем все выделенные ему ресурсы
    SDL_DestroyWindow(window);
    // Завершаем работу библиотеки и освобождаем все выделенные ей ресурсы
    SDL_Quit();
}

SDL_Texture* loadImage(char imagePath[]) {
    printf("%s\n", imagePath);
    SDL_Texture *newTexture;
    // Загружаем изображение
    SDL_Surface* loadedSurface = IMG_Load(imagePath);
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", imagePath,
        IMG_GetError());
    } else {
        // Если успешно загрузили, преобразуем загруженную поверхность в формат экранной
        // Если этого не сделать, то при каждом выводе будет проводится преобразование
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to load texture %s! SDL Error: %s\n", imagePath,
                    SDL_GetError());
        } else
            // если успешно, возвращаем указатель на поверхность с изображенеим
            return newTexture;
        // Удаляем загруженную поверхность
        SDL_FreeSurface(loadedSurface);
    }
    return NULL;
}

