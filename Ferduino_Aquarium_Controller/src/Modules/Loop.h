#pragma once // Do not change this line!
// ----------------------- main loop ------------------------------
void loop()
{
  t = rtc.getTime(); // Atualiza as variáveis que usam o RTC.

  if (executandoNuvem == false)
  {
    LED_levels_output(); // Atualiza a potência de saída dos leds
  }
  else
  {
    checkNuvemRelampago();
  }

  if (NumMins(t.hour, t.min) != lastMinute)
  {
    lastMinute = NumMins(t.hour, t.min);

    if ((haveraNuvem == true) && (executandoNuvem == false))
    {
      for (int i = 0; i < quantNuvens; i++)
      {
        if (NumMins(t.hour, t.min) == horaNuvem[i])
        {
          executandoNuvem = true;
          inicioNuvem = true;
          millis_nuvem = millis();
        }
      }
    }

    if (NumMins(t.hour, t.min) == 0)
    {
      probabilidadeNuvem(); // Calcula a probabilidade de ocorrer nuvens.
    }
  }

  checktpa();                               // Verifica e executa a TPA automática.

  if ((millis() - log_SD_millis) > 60000UL) // Executa funções a cada 1 minuto.
  {
    selecionar_SPI(SD_CARD);                // Seleciona disposito SPI que será utilizado.

    logtempgraf();                          // Grava temperatura no cartão SD.
    logphagraf();                           // Grava o PH do aquário no cartão SD.
    logphrgraf();                           // Grava o PH do reator de cálcio no cartão SD.
    logdengraf();                           // Grava densidade no cartão SD.
    logorpgraf();                           // Grava o ORP no cartão SD.
    log_SD_millis = millis();
  }

  if ((dosadoras == true) && (bitRead(tpa_status, 1) == false)) // Verifica e executa as dosagens.
  {
    if ((modo_alimentacao == false) || ((millis() < alimentacao_millis) && (modo_alimentacao == true)))
    {
      if ((millis() - dosadoras_millis) > 60000UL) // Verifica funções a cada 1 minuto.
      {
        #ifdef WATCHDOG                            // Do not change this line
          wdt_disable();
        #endif // Do not change this line
        selecionar_SPI(SD_CARD);                 // Seleciona disposito SPI que será utilizado.
        dosadoras_millis = millis();
        check_dosagem_personalizada();           // Dosagem personalizada
        #ifdef WATCHDOG                          // Do not change this line
          wdt_enable(WDTO_8S);
        #endif // Do not change this line
      }
    }
  }

  #if defined(USE_ETHERNET_SHIELD) || defined(USE_ESP8266)                        // Do not change this line
    if (bitRead(tpa_status, 1) == false)
    {
      #ifdef USE_ETHERNET_SHIELD                 // Do not change this line
        selecionar_SPI(ETHER_CARD);              // Seleciona disposito SPI que será utilizado.
        if (!MQTT.connected())
        {
          if (((millis() - millis_mqtt) > 300000UL) || (dosagem_executada == true)) // Tenta reconectar após 5 minutos ou após uma dosagem
          {
            reconnect();
            millis_mqtt = millis();
            dosagem_executada = false;
          }
        }
        MQTT.loop();
      #else // Do not change this line
        if (MQTT_connected == false)
        {
          if (((millis() - millis_mqtt) > 300000UL) || (dosagem_executada == true)) // Tenta reconectar após 5 minutos ou após uma dosagem
          {
            sincronizar();
            millis_mqtt = millis();
            dosagem_executada = false;
          }
        }
        ESP8266.Process();
      #endif // Do not change this line
    }

    if ((millis() - millis_enviar) > 120000UL)
    {
      if (bitRead(tpa_status, 1) == false)
      {
        selecionar_SPI(ETHER_CARD); // Seleciona disposito SPI que será utilizado.
        enviar_dados();
        millis_enviar = millis();
      }
    }

    if (outlets_settings == true)
    {
      if ((millis() - outlets_millis) > 300000UL) // Lê os valores da EEPROM se as configurações não forem salvas em até 5 minutos
      {
        for (byte i = 0; i < 9; i++)
        {
          outlets_changed[i] = true;
        }
        byte k = EEPROM.read(840);

        if (k != 66)
        {
          for (byte i = 0; i < 9; i++)
          {
            outlets[i] = 0;
          }
        }
        else
        {
          ler_outlets_EEPROM();
        }
        outlets_settings = false;
      }
    }
  #endif // Do not change this line

  #ifdef USE_TFT // Do not change this line
    if (myTouch.dataAvailable())
    {
      #ifdef USE_SCREENSAVER // Do not change this line
        if (((millis() - previousMillis_2) > (interval * 1000UL)) && (dispScreen == 0))
        {
          clearScreen();
          mainScreen(true);
        }
        else
        {
          processMyTouch();
        }
        previousMillis_2 = millis();
      #else // Do not change this line
        processMyTouch(); // Verifica se o LCD está sendo tocado e faz o processamento.
      #endif // Do not change this line
    }
  #endif // Do not change this line

  #ifdef WATCHDOG // Do not change this line
    wdt_reset();
  #endif // Do not change this line

  #if defined(STAMPS_EZO) || defined(STAMPS_V4X) // Do not change this line
    if ((millis() - millis_antes) >= 120000UL)   // Executa as funções a cada 2 minutos.
    {

      #ifdef USE_STAMP_FOR_CALCIUM_REACTOR // Do not change this line
        check_parametro_ph_reator();       // Verifica o pH do reator de cálcio.
      #endif // Do not change this line

      #ifdef USE_STAMP_FOR_ORP             // Do not change this line
        check_parametro_orp();             // Verifica a ORP
      #endif // Do not change this line

      #ifndef TILT_HYDROMETER              // Do not change this line
        #ifdef USE_STAMP_FOR_DENSITY       // Do not change this line
          check_parametro_densidade();     // Verifica a densidade
        #endif // Do not change this line
      #endif // Do not change this line

      #ifdef USE_STAMP_FOR_TANK_PH    // Do not change this line
        check_parametro_ph_aquario(); // Verifica o pH do aquário
      #endif // Do not change this line

      millis_antes = millis();
    }
  #endif // Do not change this line

  #if defined(USE_ETHERNET_SHIELD) || defined(USE_ESP8266) // Do not change this line
    if ((web_dosage == true) && ((millis() - millis_dosagem) > 10000UL))
    {
      #ifdef WATCHDOG // Do not change this line
        wdt_disable();
      #endif // Do not change this line
      dosagem_manual();
      web_dosage = false;
      #ifdef WATCHDOG // Do not change this line
        wdt_enable(WDTO_8S);
      #endif // Do not change this line
    }

    if ((web_calibracao == true) && ((millis() - millis_dosagem) > 10000UL))
    {
      #ifdef WATCHDOG // Do not change this line
        wdt_disable();
      #endif // Do not change this line
      calibrar();
      web_calibracao = false;
      #ifdef WATCHDOG // Do not change this line
        wdt_enable(WDTO_8S);
      #endif // Do not change this line
    }
  #endif // Do not change this line

  #if defined(RFM12B_LED_CONTROL) || defined(RFM12B_RELAY_CONTROL) // Do not change this line
    if ((millis() - lastPeriod_millis) > 5000UL)
    {
      if (millis() > 20000UL)
      {
        selecionar_SPI(RFM);        // Seleciona disposito SPI que será utilizado.
        #ifdef RFM12B_LED_CONTROL   // Do not change this line
          RF_LED();
        #endif // Do not change this line
        #ifdef RFM12B_RELAY_CONTROL // Do not change this line
          RF_RELAY();
        #endif // Do not change this line
      }
      lastPeriod_millis = millis();
    }
  #endif // Do not change this line

  if ((dispScreen != 22) && (web_teste == false))
  {
    teste_em_andamento = false;
  }

  #if defined(USE_ETHERNET_SHIELD) || defined(USE_ESP8266) // Do not change this line
    if (web_teste == true)
    {
      if ((millis() - teste_led_millis) > 600000UL)
      {
        web_teste = false;
        teste_em_andamento = false;
        ler_predefinido_EEPROM();
      }
    }
  #endif // Do not change this line

  #ifdef USE_TFT                                    // Do not change this line
    if ((dispScreen == 3) && (LEDtestTick == true)) // Imprime valores se o teste de todos os leds em andamento.
    {
      testScreen();
    }
  #endif // Do not change this line

  Wavemaker();

  #ifdef USE_TFT          // Do not change this line
    if (dispScreen == 10) // Desenha os gráficos enquanto o menu estiver aberto.
    {
      if ((modo_alimentacao == false) || (wavemaker_on_off == false))
      {
        Grafico_WaveMaker();
      }
    }
  #endif // Do not change this line

  if (modo_alimentacao == true) // Função que desliga alimentador automático.
  {
    if (millis() >= alimentacao_millis)
    {
      if ((millis() - alimentacao_millis) > (duracao_alimentacao * 1000UL))
      {
        modo_alimentacao = false;
        #ifdef USE_TFT // Do not change this line
          if (dispScreen == 45)
          {
            strcpy_P(buffer, (char*)pgm_read_word_near(&(tabela_textos[17])));
            printButton(buffer, anT[0], anT[1], anT[2], anT[3]); // tabela_textos[17] = "INICIAR"
          }
        #endif // Do not change this line
      }
    }
  }

  if (wavemaker_on_off == true) // Função que liga o wavemaker após o período de alimentação.
  {
    if ((millis() - wavemaker_on_off_millis) > (desligar_wavemaker * 60000UL))
    {
      wavemaker_on_off = false;
    }
  }

  if ((millis() - check_alimentador_millis) > 60000UL)
  {
    selecionar_SPI(SD_CARD);
    check_alimentador(); // Verifica agendamendo do alimentador.
    check_alimentador_millis = millis();
  }

  alimentador_automatico();                        // Liga ou desliga o alimentador automático.

  if (millis() - previousMillis > 5000UL)          // Verifica as funções a cada 5 segundos.
  {
    checkTempC();                                  // Verifica as temperaturas.
    reposicao_agua_doce();                         // Verifica se há a necessidade reposição da água doce.
    check_nivel();                                 // Verifica se há algum problema com os níveis dos aquários.
    check_alarme();                                // Verifica os alarmes.
    check_temporizadores();                        // Ativa ou desativa os timers.

    #if defined(STAMPS_EZO) || defined(STAMPS_V4X) // Do not change this line

      #ifdef USE_STAMP_FOR_CALCIUM_REACTOR         // Do not change this line
        check_PH_reator();                         // Verifica o pH do reator de cálcio.
      #endif // Do not change this line

      #ifdef USE_STAMP_FOR_ORP                     // Do not change this line
        check_ORP();                               // Verifica a ORP
      #endif // Do not change this line

      #ifndef TILT_HYDROMETER                      // Do not change this line
        #ifdef USE_STAMP_FOR_DENSITY               // Do not change this line
          check_densidade();                       // Verifica a densidade
        #endif // Do not change this line
      #endif // Do not change this line

      #ifdef USE_STAMP_FOR_TANK_PH // Do not change this line
        check_PH_aquario();        // Verifica o pH do aquário
      #endif // Do not change this line

    #endif // Do not change this line

    #ifdef DISABLE_SKIMMER              // Do not change this line
      check_level_skimmer();            // desativa o skimmer para evitar transbordamento.
    #endif // Do not change this line

    if (LEDtestTick == false)           // Atualiza se o teste de todos os leds não estiver em andamento.
    {
      min_cnt = NumMins(t.hour, t.min); // Atualiza o intervalo para determinar a potência dos leds.
    }

    #ifdef USE_TFT                      // Do not change this line
      if (dispScreen == 0)
      {
        #ifdef USE_SCREENSAVER          // Do not change this line
          if ((millis() - previousMillis_2) > (interval * 1000UL))
          {
            clockScreen();              // atualiza o protetor de tela
          }
          else
          {
            mainScreen(); // Atualiza tela inicial.
            firstTime = true;
          }
        #else // Do not change this line
          mainScreen(); // Atualiza tela inicial.
        #endif // Do not change this line
      }
    #endif // Do not change this line

    if (suavizar <= 1)
    {
      suavizar += 0.1;
    }

    LOG(F("Free memory: "));
    LOGLN(FreeStack());

    previousMillis = millis();
  }

} // void loop()
