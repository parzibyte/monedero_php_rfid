<?php
/*

  ____          _____               _ _           _       
 |  _ \        |  __ \             (_) |         | |      
 | |_) |_   _  | |__) |_ _ _ __ _____| |__  _   _| |_ ___ 
 |  _ <| | | | |  ___/ _` | '__|_  / | '_ \| | | | __/ _ \
 | |_) | |_| | | |  | (_| | |   / /| | |_) | |_| | ||  __/
 |____/ \__, | |_|   \__,_|_|  /___|_|_.__/ \__, |\__\___|
         __/ |                               __/ |        
        |___/                               |___/         
    
____________________________________
/ Si necesitas ayuda, contáctame en \
\ https://parzibyte.me               /
 ------------------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
Creado por Parzibyte (https://parzibyte.me).
------------------------------------------------------------------------------------------------
            | IMPORTANTE |
Si vas a borrar este encabezado, considera:
Seguirme: https://parzibyte.me/blog/sigueme/
Y compartir mi blog con tus amigos
También tengo canal de YouTube: https://www.youtube.com/channel/UCroP4BTWjfM0CkGB6AFUoBg?sub_confirmation=1
Twitter: https://twitter.com/parzibyte
Facebook: https://facebook.com/parzibyte.fanpage
Instagram: https://instagram.com/parzibyte
Hacer una donación vía PayPal: https://paypal.me/LuisCabreraBenito
------------------------------------------------------------------------------------------------
*/ ?>
<?php include_once "encabezado.php"; ?>
<section class="section">
    <div class="columns">
        <div class="column has-text-centered">
            <h1 class="is-size-1">Recargar saldo</h1>
            <p class="control">
                <input class="input" autofocus placeholder="Escribe el saldo" type="number" id="inputSaldo">
            </p>
            <br>
            <button id="botonRecargar" class="button is-success">Recargar</button>
            <p id="estado"></p>
        </div>
    </div>
    <script>
    // Similar a "comprar.php" pero ahora llamamos a "establecer_modo_recargar"
        const $botonRecargar = document.querySelector("#botonRecargar"),
            $inputSaldo = document.querySelector("#inputSaldo"),
            $estado = document.querySelector("#estado");
        $botonRecargar.onclick = async () => {
            const saldo = parseFloat($inputSaldo.value);
            if (!saldo) {
                return alert("Escribe el saldo");
            }
            $estado.textContent = "Estableciendo conexión con servidor...";
            const respuestaRaw = await fetch("./establecer_modo_recargar.php?saldo=".concat(saldo));
            const respuestaJson = await respuestaRaw.json();
            if (respuestaJson) {
                $estado.textContent = "OK. Coloque la tarjeta en el lector";
            } else {

                $estado.textContent = "Error estableciendo comunicación con servidor";
            }
            const esperarLector = async () => {
                $botonRecargar.disabled = true;
                if (!$botonRecargar.classList.contains("is-loading")) {
                    $botonRecargar.classList.add("is-loading");
                }
                $estado.textContent = "Verificando...";
                const respuestaRaw = await fetch("./ver_mensaje_lector.php");
                const texto = await respuestaRaw.text();
                if (texto) {
                    $estado.textContent = texto;
                    $inputSaldo.value = "";
                    $botonRecargar.disabled = false;
                    $botonRecargar.classList.remove("is-loading");
                } else {
                    $estado.textContent = "No se ha leído nada. Recuerde colocar la tarjeta en el lector. Intentando de nuevo...";
                    setTimeout(esperarLector, 2000);
                }
            };
            esperarLector();
        };
    </script>
</section>
<?php include_once "pie.php"; ?>