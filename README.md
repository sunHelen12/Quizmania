<h1 align="center">⁉️ Quizmania - Projeto Final - Embarcatech 🕹️</h1> 
    <h2>Descrição Geral</h2>
      <p>O QuizMania é um jogo de perguntas e respostas desenvolvido para a plataforma educacional <i>BitDogLab</i> com a placa Raspberry Pi Pico. Esse game
        tem como propósito combinar entretenimento e educação, proporcionando uma experiência interativa e dinâmica para os participantes. Com um formato 
        competitivo para dois jogadores, o jogo estimula o aprendizado de forma lúdica, incentivando o raciocínio rápido e a tomada de decisões sob pressão.
        Além disso, a mecânica baseada em perguntas de conhecimento geral permite que os jogadores testem seus conhecimentos e reforcem informações de maneira 
        envolvente e divertida.</p>
     <h2>Componentes do Projeto</h2>
        <ul>
          <li><strong>BitDogLab:</strong> Utilizado para entrada de dados, com cada tecla mapeada para uma função específica.</li>
          <li><strong>Display OLED SSD1306:</strong> Exibe os textos do jogo.</li>
          <li><strong>Matriz de LEDs WS2812 (5x5):</strong> Exibe efeitos divertidos e as contagens regressivas.</li>
          <li><strong>Botões Físicos (A e B):</strong> Define o jogador que irá responder a pergunta e direciona a pontuação do jogo.</li>
          <li><strong>Buzzer:</strong> Emite sinais sonoros para divertir e alertar os jogadores.</li>
          <li><strong>LED RGB (Verde e Vermelho):</strong></li>
            <ul>
              <li><strong>Verde:</strong> Aparece quando os jogadores devem apertar os botões.</li>
              <li><strong>Vermelho:</strong> Aparece quando o tempo para responder a pergunta acaba.</li>
            </ul>
        </ul> 
    <h2>Instruções de Jogabilidade 🎮</h2>
      <h3>Início do jogo</h3>
         <ul>
           <li>1. Tela de Introdução:</li>
            <ul>
              <li>Ao ligar a placa BitDogLab, a tela de introdução será exibida no Display OLED.</li>
              <li>Pressione o botão do joystick para iniciar o jogo.</li>
             </ul>
          </ul> 
      <h3>Preparação para o Jogo</h3>
          <ul>
           <li>2. Tela de Jogadores: </li>
            <ul>
              <li>O display OLED mostrará quais botões correspondem a cada jogador:</li>
              <ul>
                <li><strong>Jogador 1:</strong> Botão A</li>
                <li><strong>Jogador 2:</strong> Botão B</li>
              </ul>
              <li>Os jogadores devem se posicionar e estar prontos para pressionar seus botões.</li>
             </ul>
          </ul>  
        <h3>Início da Rodada</h3>
         <ul>
           <li>3. Contagem Regressiva:</li>
            <ul>
              <li>O jogo exibirá uma contagem regressiva na matriz de LEDs quando está iniciando.</li>
              <li>Após isso será exibito uma tela avisando aos jogadores  que se preparem, pois a tela de aperte os botões está vindo.</li>
              <li>Assim que aparecer a tela para apertar os botões, cada jogador deve apertar seus botões e aguardar a leitura para saber quem irá responder a pergunta.</li>    
             </ul>
          </ul> 
          <ul>
           <li>4. Pergunta:</li>
            <ul>
              <li>A pergunta será exibida no display OLED.</li>              
             </ul>
          </ul>
       <h3>Resposta e Pontuação</h3>
          <ul>
           <li>5. Tempo para Responder: </li>
            <ul>
              <li>Após alguns segundos, o LED vermelho piscará e o buzzer emitirá um sinal de alerta, indicando que o tempo para responder está acabando.</li>
              <li>Logo, a resposta correta será exibida no display OLED.</li>
             </ul>
          </ul>  
          <ul>
           <li>6. Acerto ou Erro:</li>
            <ul>
              <li>Se o jogador que respondeu acertar, ele deve pressionar seu botão novamente para acumular o ponto.</li>
              <li>Se ele errar ou não responder a tempo, o outro jogador deve pressionar seu botão para acumular o ponto.</li>
             </ul>
          </ul>
        <h3>Finalização da Rodada</h3>
          <ul>
           <li>7. Placar: </li>
            <ul>
              <li>O placar atualizado será exibido no display OLED após cada rodada.</li>
            </ul>
          </ul>
        <h3>Fim do Jogo</h3>
          <ul>
           <li>8. Placar Final: </li>
            <ul>
              <li>O placar atualizado será exibido no display OLED após cada rodada.</li>
              <li>O jogo retorna para tela inicial e pode ser reiniciado pressionando o botão do joystick novamente.</li>
            </ul>
          </ul>    
     <h3>Dicas</h3>
          <ul>
              <li><strong>Concentração:</strong>  Esteja atento tanto a tela de apertar os botões quanto às perguntas.</li>
              <li><strong>Diversão:</strong> O jogo é projetado para ser divertido e educativo, então aproveite!</li>          
          </ul>   
    <h3>Modificação de Perguntas</h3>
          <ul>
              <li>As perguntas podem ser facilmente modificadas no código do jogo para adaptar o conteúdo ao público-alvo ou ao tema desejado.</li>            
          </ul>        
  <h2>Como Executar o Projeto</h2>
        <h3>Clone o Repositório</h3>
            <ol>
                <li>Abra o <strong>Prompt de Comando</strong> ou o terminal de sua preferência.</li>
                <li>Clone o repositório usando o Git:
                    <pre><code>git clone https://github.com/seu-usuario/seu-repositorio.git</code></pre>
                </li>
                <li>Entre no diretório do projeto:
                    <pre><code>cd seu-repositorio</code></pre>
                </li>
            </ol>    
        <h3>Configuração do Ambiente Local</h3>
                <ol>
                    <li>Baixe e instale o <a href="https://cmake.org/download/" target="_blank">CMake</a>.</li>
                    <li>Configure o <strong>Pico SDK</strong> seguindo o guia oficial em 
                        <a href="https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf" target="_blank">
                            Raspberry Pi Pico SDK</a>.
                    </li>
                    <li>Crie um diretório de construção:
                        <pre><code>mkdir build</code></pre>
                        <pre><code>cd build</code></pre>
                    </li>
                    <li>Execute o CMake para gerar os arquivos de construção:
                        <pre><code>cmake ..</code></pre>
                    </li>
                </ol>
        <h3>Compilar o Projeto</h3>
                <p>Após configurar o ambiente, compile o projeto executando o seguinte comando dentro do diretório <code>build</code>:</p>
                <pre><code>make</code></pre>
                <p>Isso criará o arquivo binário do programa, geralmente no formato <code>.uf2</code>.</p>
   <h2>Mensagem</h2>
        <p><pre>Espero que vocês se divirtam jogando Quizmania! Se tiverem alguma dúvida ou precisarem de ajuda, sintam-se à vontade para entrar em contato.</pre></p>
