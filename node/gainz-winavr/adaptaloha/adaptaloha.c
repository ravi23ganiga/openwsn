

int main()
{
    return 0;
}

int adaptive_aloha_tx_node()
{
    TiIoBuf * txbuf;

    cc2420_construct
    adaptive_aloha_construct

    cc2420_open;
    adaptive_aloha_open( cc2420_provide(cc) );

    while (1)
    {
        adaptive_aloha_send( txbuf );
        delay( 20 );
    }

    adaptive_aloha_close();
    cc2420_close();
    
    adaptive_aloha_destroy();
    cc2420_destroy();    
}
