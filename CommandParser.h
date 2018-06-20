#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

class CommandParser {
  private:
    #define MAX_CMD_SIZE 30
    String cmd;
    char c;
    uint8_t cmd_complete, i;
    int8_t index, last_index;
    char delimiter;
    
    #define MAX_COMANDO_SIZE 10
    String comando;
    
    #define MAX_ARGS 3
    String argumentos[MAX_ARGS];

    #define MAX_COMMANDS 5
    struct {
      String command;
      void (*action)(String *args);
    } commands[MAX_COMMANDS];
    uint8_t commands_index;
    
    void clearArgumentos(void) {
      for(i = 0; i < MAX_ARGS; i++) {
        argumentos[i] = "";
      }
    }

    void clearCommands(void) {
      for(i = 0; i < MAX_COMMANDS; i++) {
        commands[i].command = "";
        commands[i].action = NULL;
      }
      commands_index = 0;
    }

  public:
    CommandParser() {
      cmd.reserve(MAX_CMD_SIZE);
      cmd = "";
      cmd_complete = 0;
      comando.reserve(MAX_COMANDO_SIZE);
      comando = "";
      for (i = 0; i < MAX_ARGS; i++) {
        argumentos[i].reserve((MAX_CMD_SIZE - MAX_COMANDO_SIZE) / MAX_ARGS);
      }
      clearArgumentos();
      for(i = 0; i < MAX_COMMANDS; i++) {
        commands[i].command.reserve(MAX_COMANDO_SIZE);
      }
      clearCommands();
      delimiter = ' ';
    }

    void addCommand(String str, void (*action)(String *args)) {
      if(commands_index < MAX_COMMANDS) {
        commands[commands_index].command = str;
        commands[commands_index].action = action;
        commands_index++;
      }
    }

    void setDelimiter(char c) {
      this->delimiter = c;
    }

    void run(void) {
      if(Serial.available() > 0) {
        c = Serial.read(); // leemos un caracter
        switch(c) {
          // final de linea
          case '\r':
          case '\n':
            if (cmd != "") {
              cmd_complete = 1;
            }
            break;
          // añadimos caracteres
          default:
            cmd += c;
            break;
        }
        if(cmd_complete) { // procesamos el comando
          cmd_complete = 0;
          cmd.trim(); // quitamos valores innecesarios
          cmd += delimiter; // requerido para detectar el ultimo argumento
    
          // comando
          index = cmd.indexOf(delimiter); // usamos el espacio como delimitador
          if(index > 0) { // si se encuentra el delimitador index > 0
            comando = cmd.substring(0, index);
    
            // argumentos
            i = 0;
            while(i < MAX_ARGS) {
              last_index = index + 1; 
              index = cmd.indexOf(delimiter, last_index);
              if(index > 0) {
                argumentos[i] = cmd.substring(last_index, index);
                i++;
              }
              else {
                i = MAX_ARGS;
              }
            }
          }

          for(i = 0; i < MAX_COMMANDS; i++) {
            if(commands[i].command == comando) {
              commands[i].action(argumentos);
            }
          }
          
          cmd = "";
          comando = "";
          clearArgumentos();
        }
      }
    }
};

#endif

