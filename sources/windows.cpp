#include <SFML/Graphics.hpp> // SFML
#include <unistd.h>
#include <iostream>
#include <vector>


extern std::vector<unsigned short> _temp_data;

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}



// === ПАЛИТРА С ИНТЕРНЕТА ===
typedef struct {
    double r,g,b;
} COLOUR;

COLOUR GetColour(double v,double vmin,double vmax)
{
   COLOUR c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
   } else {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
   }

   return(c);
}
// === ===



void DrawAnotherMethodColors(sf::VertexArray &Map)
{
    sf::Color CurrentColor(255,255,255);
    uint8_t buffer[327680] = {0};

    int tmin, tmax;
    tmin = 200;
    tmax = 0;

    for(int i=0; i < 327630; i++)
    {
        buffer[i] = (_temp_data[i] + 7000)/30 - 273.2;
    }


    for(int i=0; i < 327630; i++)
    {
        if(buffer[i] < tmin)
            tmin = buffer[i];

        if(buffer[i] > tmax)
            tmax = buffer[i];

        if(buffer[i] > 50)
            std::cout << "Странное значение, " << i << "=" << buffer[i] << std::endl;
    }

    std::cout << "tmax = " << tmax << std::endl;
    std::cout << "tmin = " << tmin << std::endl;
    printf("Возврат AV: %d \n", avcodec_version());

    int dv = tmax - tmin;
    COLOUR mycolor;

    for(int i=0; i < 327600; i++)
    {
        mycolor = GetColour(buffer[i], tmin, tmax);
        if(i==10000)
        {
            std::cout << "mycolor_r: " << mycolor.r << std::endl;
            std::cout << "mycolor_g: " << mycolor.g << std::endl;
            std::cout << "mycolor_b: " << mycolor.b << std::endl;
        }
        CurrentColor.r = mycolor.r*255;
        CurrentColor.g = mycolor.g*255;
        CurrentColor.b = mycolor.b*255;

        Map[i].color = CurrentColor;

    }


    int x=0, y=0;

    for(int i=0; i < 327630; i++)
    {
        Map[i].position = sf::Vector2f(x, y);
        x++;
        if(x > 639)
        {
            x=0;
            y++;
        }

        
    }

    /*

    if (v < (vmin + 0.25 * dv))
    {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
    }
    else if (v < (vmin + 0.5 * dv))
    {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
    }
    else if (v < (vmin + 0.75 * dv))
    {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
    }
    else
    {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
    }
    */

}




void DrawChubkoColors(sf::VertexArray &Map)
{   
    sf::Color CurrentColor(255,255,255);
    uint8_t buffer[327680] = {0};

    int tmin, tmax;
    tmin = 200;
    tmax = 0;
    
    //std::cout << "Значение в буфере: " << (_temp_data[0]+ 7000)/30 - 273.2 << std::endl;

    for(int i=0; i < 327630; i++)
    {
        buffer[i] = (_temp_data[i] + 7000)/30 - 273.2;
        //printf("Buffer %d = %d\n", i, buffer[i]);
        
    }

    for(int i=0; i < 327630; i++)
    {
        if(buffer[i] < tmin)
            tmin = buffer[i];

        if(buffer[i] > tmax)
            tmax = buffer[i];

        if(buffer[i] > 50)
            std::cout << "Странное значение, " << i << "=" << buffer[i] << std::endl;
    }
    
    
    std::cout << "tmax = " << tmax << std::endl;
    std::cout << "tmin = " << tmin << std::endl;
    
    int color;

    for(int i=0; i < 327600; i++)
    {

        CurrentColor.r = 0;
        CurrentColor.g = 0;
        CurrentColor.b = 0;

        //std::cout << "data: " << (int) data[i] << std::endl;
        color = buffer[i];
        color -= tmin;
        float coef_auto_scale = 256/(tmax - tmin);
        color = color * coef_auto_scale;
        if(color < 127)
        {
            CurrentColor.b = 255 - (color/8);
        }
        if(color > 127) 
        {
            CurrentColor.r = color/8;
        }
        
        
        if((color < 191) && (color > 127))
        {
            CurrentColor.g = 15 - (color-127)/4;
        }

        if ((color > 64) && (color < 127)) 
        {
            CurrentColor.g = (color / 4) - 16;
        }
        if (color == 127)
        {
            CurrentColor.g = 15;
        }
        //CurrentColor.r = color*2;
        //CurrentColor.g = 90;
        //CurrentColor.b = 60;


        //std::cout << "color: " << (int) color << "   DATA: " << (unsigned char) data[i] << std::endl;
        //printf("color: %02x \t data: %02x \n", color, _temp_data[i]);
        //usleep(100);

        CurrentColor.r *= 16;
        CurrentColor.g *= 16;
        CurrentColor.b *= 16;

        if(i == 10000)
        {
            printf("1: r=%x\n", CurrentColor.r);
            printf("2: g=%x\n", CurrentColor.g);
            printf("3: b=%x\n", CurrentColor.b);

            //std::cout << "1: r=" << CurrentColor.r << std::endl;
            //std::cout << "2: g=" << CurrentColor.g << std::endl;
            //std::cout << "3: b=" << CurrentColor.b << std::endl;
        }

        Map[i].color = CurrentColor;

    }


    int x=0, y=0;

    for(int i=0; i < 327630; i++)
    {
        Map[i].position = sf::Vector2f(x, y);
        x++;
        if(x > 639)
        {
            x=0;
            y++;
        }

        
    }
}


void * WindowThread(void * args)
{
    sf::RenderWindow view_window;
    view_window.create(sf::VideoMode(580, 600), "View Window");

    sf::VertexArray ThermalMap(sf::Points, 327680);

    while(view_window.isOpen())
    {
        sf::Event event;

        while(view_window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                view_window.close();
            
            if(event.type == sf::Event::KeyPressed)
            {
                std::cout << "Нажата: " << event.key.code << std::endl;
               
                
                //DrawChubkoColors(ThermalMap);
                DrawAnotherMethodColors(ThermalMap);
                

                view_window.clear(sf::Color::Black); // отрисовка в скрытый буфер
                view_window.draw(ThermalMap);
                view_window.display();
            }
        }
        //sleep(0.5);
    }
}

void DrawMap(std::array<uint8_t, 327680> data)
{
    std::cout << "\tОтрисовка окна с картой" << std::endl;

    int tmin, tmax;
    tmin = 255;
    tmax = 0;

    for(int i=0; i < 327680; i++)
    {
        if(data[i] < tmin)
            tmin = data[i];

        if(data[i] > tmax)
            tmax = data[i];
    }

    std::cout << "tmax = " << tmax << std::endl;
    std::cout << "tmin = " << tmin << std::endl;

    sf::RenderWindow map_window;
    sf::VertexArray ThermalMap(sf::Points, 327680);
    sf::Color CurrentColor(0,0,255);

    int color;

    for(int i=0; i < 327680; i++)
    {
        //std::cout << "data: " << (int) data[i] << std::endl;
        color = data[i];
        color -= tmin;
        float coef_auto_scale = 256/(tmax - tmin);
        color = color * coef_auto_scale;
        if(color < 127)
        {
            CurrentColor.b = 255 - (color/8);
        }
        if(color > 127) 
        {
            CurrentColor.r = color/8;
        }
        
        if((color < 191) && (color > 127))
            CurrentColor.g = 15 - (color-127)/4;
        if ((color > 64) && (color < 127)) 
            CurrentColor.g = (color / 4) - 16;
        if (color == 127)
            CurrentColor.g = 15;
        
        //CurrentColor.r = color*2;
        //CurrentColor.g = 90;
        //CurrentColor.b = 60;


        //std::cout << "color: " << (int) color << "   DATA: " << (unsigned char) data[i] << std::endl;
        printf("color: %02x \t data: %02x \n", color, data[i]);
        //usleep(100);

        CurrentColor.r *= 16;
        CurrentColor.g *= 16;
        CurrentColor.b *= 16;

        ThermalMap[i].color = CurrentColor;

    }
  
    

    int x=0, y=0;

    for(int i=0; i < 327680; i++)
    {
        ThermalMap[i].position = sf::Vector2f(x, y);
        x++;
        if(x > 639)
        {
            x=0;
            y++;
        }

        
    }

    map_window.create(sf::VideoMode(700, 600), "Map Viewer");
    map_window.clear(sf::Color::Black); // отрисовка в скрытый буфер
    map_window.draw(ThermalMap);
    map_window.display(); 
    sleep(15);

}





