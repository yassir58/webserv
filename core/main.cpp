/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelatman <yelatman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 13:04:37 by yelatman          #+#    #+#             */
/*   Updated: 2023/02/23 15:27:28 by yelatman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerInstance.hpp"
#include "../config/config.hpp"
#include "../request/request.h"
#include "../request/Request.hpp"


int main (int argc , char *argv[])
{

    HttpApplication *app;
    app = new HttpApplication ();
    try 
    {
        app->handleConfig (argc, argv);
    }
    catch (Parse_error &err)
    {
        std::cout << err.what () << std::endl;
        exit (EXIT_FAILURE);
    }
    catch (std::exception &exc)
    {
        std::cout << exc.what () << std::endl;
        app->deleteConfig ();
        // system ("leaks webserv");
        exit (EXIT_FAILURE);
    }
    
    try
    {
        handleSignals ();
		app->handleSigPipe ();
        app->filterServerBlocks ();
        app->connectServers ();
		app->initServerSet ();
        while (1)
        {
            try 
            {
                app->checkForConnection ();
            }
            catch (Fatal_error &exc)
            {
                std::cout << exc.what () << std::endl;
                delete app;
                // system ("leaks webserv");
                exit (EXIT_FAILURE);
            }
            catch (std::exception &exc)
            {
                std::cout << exc.what () << std::endl;
				 // handle error
            }
        }    
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
        delete app;
        exit (EXIT_FAILURE);
    }
    return (0);
}
