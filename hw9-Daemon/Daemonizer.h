//
// Created by daniilsmelskiy on 23.02.2022.
//

#ifndef OTUSHW_DAEMONIZER_H
#define OTUSHW_DAEMONIZER_H

#endif //OTUSHW_DAEMONIZER_H

/*
 * PID File file descriptor
 */
static int PIDFileFD;

/*
 * Lock PID file and verify that daemon isn't already running
 */
void LockPIDFile();

/*
 * Daemonize current program
 */
void Daemonize();
