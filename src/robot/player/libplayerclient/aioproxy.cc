/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000-2003
 *     Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
 *     Nik Melchior
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* AIOProxy client library.  Based loosly on the DIO Proxy */
#include <playerclient.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

void
AIOProxy::FillData(player_msghdr_t hdr, const char *buffer)
{
  if(hdr.size != sizeof(player_aio_data_t)) 
  {
    if(player_debug_level(-1) >= 1)
      fprintf(stderr,"WARNING: AIOProxy expected %d bytes of "
              "analog data, but received %d. Unexpected results may "
              "ensue.\n",
              sizeof(player_aio_data_t),hdr.size);
  }

  count = ((player_aio_data_t *)buffer)->count;
  if (count<0)
  {
	  printf("WARNING: AIOProxy received a negative count value.\n");
	  count=0;
  }
  for (int i=0;i<count;i++)
	anin[i] = ntohl(((player_aio_data_t *)buffer)->anin[i]);
}

// interface that all proxies SHOULD provide
void
AIOProxy::Print()
{
  printf("#AIO(%d:%d) - %c\n", m_device_id.code,
           m_device_id.index, access);
  printf("Count: %i\n",count);
  if (count<0)
  {
	printf("WARNING: AIOProxy received a negative count value.\n");
   	count=0;
  }
  for (int i=0;i<count;i++)
  {
	  printf("AIO%i - %i\n",i, anin[i]);
  }
}

