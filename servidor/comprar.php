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
            <h1 class="is-size-1">Comprar</h1>
            <table class="table">
                <thead>
                    <tr>
                        <th>Nombre</th>
                        <th>Precio</th>
                        <th>Comprar</th>
                    </tr>
                </thead>
                <!-- 
                Guardamos el precio del producto en el data-precio
                -->
                <tbody>
                    <tr>
                        <td>Galletas</td>
                        <td>10.5</td>
                        <td>
                            <button class="button is-info comprar" data-precio="10.5">Comprar</button>
                        </td>
                    </tr>
                    <tr>
                        <td>Champú</td>
                        <td>5000</td>
                        <td>
                            <button class="button is-info comprar" data-precio="5000">Comprar</button>
                        </td>
                    </tr>
                    <tr>
                        <td>Audífonos</td>
                        <td>450</td>
                        <td>
                            <button class="button is-info comprar" data-precio="450">Comprar</button>
                        </td>
                    </tr>
                    <tr>
                        <td>Mi nuevo producto</td>
                        <td>2000</td>
                        <td>
                            <button class="button is-info comprar" data-precio="2000">Comprar</button>
                        </td>
                    </tr>
                     <tr>
                        <td>Mi otrooo producto</td>
                        <td>123</td>
                        <td>
                            <button class="button is-info comprar" data-precio="123">Comprar</button>
                        </td>
                    </tr>

                    
                </tbody>
            </table>
            <p id="estado"></p>
        </div>
    </div>
    <script>
        const $botonesComprar = document.querySelectorAll(".comprar"),
            $estado = document.querySelector("#estado");
        // Si hacen clic en un botón para comprar...
        for (const $boton of $botonesComprar) {
            $boton.onclick = async () => {
                // Extraemos precio y le decimos al servidor que se ponga en modo "descontar saldo" tan pronto el lector lo "contacte"
                const precio = parseFloat($boton.dataset.precio);
                $estado.textContent = "Estableciendo conexión con servidor...";
                const respuestaRaw = await fetch("./establecer_modo_compra.php?saldo=".concat(precio));
                const respuestaJson = await respuestaRaw.json();
                if (respuestaJson) {
                    $estado.textContent = "OK. Coloque la tarjeta en el lector";
                } else {

                    $estado.textContent = "Error estableciendo comunicación con servidor";
                }
                // Ahora el servidor ya está en espera de que el lector "le hable". Así que consultamos cada 2 segundos si hay novedades
                const esperarLector = async () => {
                    // Deshabilitamos todos los botones, pues estamos esperando
                    $botonesComprar.forEach($boton => {
                        $boton.disabled = true;
                        if (!$boton.classList.contains("is-loading")) {
                            $boton.classList.add("is-loading");
                        }
                    });
                    // Verificamos...
                    $estado.textContent = "Verificando...";
                    const respuestaRaw = await fetch("./ver_mensaje_lector.php");
                    const texto = await respuestaRaw.text();
                    // Si se devuelve una cadena no vacía, entonces la mostramos (la cadena puede tener un mensaje de éxito o de error)
                    if (texto) {
                        $estado.textContent = texto;

                        $botonesComprar.forEach($boton => {
                            $boton.disabled = false;
                            $boton.classList.remove("is-loading")
                        });
                    } else {
                        // Si el servidor indica que el lector no se ha comunicado, lo indicamos y volvemos a intentar dentro de 2 segundos
                        $estado.textContent = "No se ha leído nada. Recuerde colocar la tarjeta en el lector. Intentando de nuevo...";
                        setTimeout(esperarLector, 2000);
                    }
                };
                esperarLector();
            };
        }
    </script>
</section>
<?php include_once "pie.php"; ?>