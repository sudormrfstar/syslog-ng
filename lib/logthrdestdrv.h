/*
 * Copyright (c) 2013, 2014 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2013, 2014 Gergely Nagy <algernon@balabit.hu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#ifndef LOGTHRDESTDRV_H
#define LOGTHRDESTDRV_H

#include "syslog-ng.h"
#include "driver.h"
#include "stats.h"
#include "logqueue.h"
#include "mainloop-worker.h"
#include <iv.h>
#include <iv_event.h>

typedef struct _LogThrDestDriver LogThrDestDriver;

struct _LogThrDestDriver
{
  LogDestDriver super;

  StatsCounterItem *dropped_messages;
  StatsCounterItem *stored_messages;

  time_t time_reopen;

  LogQueue *queue;

  /* Worker stuff */
  struct
  {
    void (*thread_init) (LogThrDestDriver *s);
    void (*thread_deinit) (LogThrDestDriver *s);
    gboolean (*insert) (LogThrDestDriver *s);
    void (*disconnect) (LogThrDestDriver *s);
  } worker;

  struct
  {
    gchar *(*stats_instance) (LogThrDestDriver *s);
    gchar *(*persist_name) (LogThrDestDriver *s);
  } format;
  gint stats_source;
  gint32 seq_num;

  void (*queue_method) (LogThrDestDriver *s);
  WorkerOptions worker_options;
  struct iv_event wake_up_event;
  struct iv_event shutdown_event;
  struct iv_timer timer_reopen;
  struct iv_timer timer_throttle;
  struct iv_task  do_work;
};

gboolean log_threaded_dest_driver_deinit_method(LogPipe *s);
gboolean log_threaded_dest_driver_start(LogPipe *s);

void log_threaded_dest_driver_init_instance(LogThrDestDriver *self, GlobalConfig *cfg);
void log_threaded_dest_driver_free(LogPipe *s);

void log_threaded_dest_driver_suspend(LogThrDestDriver *self);

void log_threaded_dest_driver_message_accept(LogThrDestDriver *self,
                                             LogMessage *msg,
                                             LogPathOptions *path_options);
void log_threaded_dest_driver_message_drop(LogThrDestDriver *self,
                                           LogMessage *msg,
                                           LogPathOptions *path_options);

#endif
