/*
 * Copyright (C) 2000, 2001, 2002, 2003 Håkan Hjort <d95hjort@dtek.chalmers.se>
 *
 * Much of the contents in this file is based on VOBDUMP.
 *
 * VOBDUMP: a program for examining DVD .VOB filse
 *
 * Copyright 1998, 1999 Eric Smith <eric@brouhaha.com>
 *
 * VOBDUMP is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.  Note that I am not
 * granting permission to redistribute or modify VOBDUMP under the
 * terms of any later version of the General Public License.
 *
 * This program is distributed in the hope that it will be useful (or
 * at least amusing), but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>

#if !defined(_XBOX)
#include <inttypes.h>

#endif
//#include <assert.h>

#include "config.h" // Needed for WORDS_BIGENDIAN
#include "nav_types.h"
#include "nav_print.h"
#include "dvdread_internal.h"
#include "../mp_msg.h"

static void print_time(dvd_time_t *dtime) {
  const char *rate;
  CHECK_VALUE((dtime->hour>>4) < 0xa && (dtime->hour&0xf) < 0xa);
  CHECK_VALUE((dtime->minute>>4) < 0x7 && (dtime->minute&0xf) < 0xa);
  CHECK_VALUE((dtime->second>>4) < 0x7 && (dtime->second&0xf) < 0xa);
  CHECK_VALUE((dtime->frame_u&0xf) < 0xa);
  
  mp_msg(0,0,"%02x:%02x:%02x.%02x", 
	 dtime->hour,
	 dtime->minute,
	 dtime->second,
	 dtime->frame_u & 0x3f);
  switch((dtime->frame_u & 0xc0) >> 6) {
  case 1:
    rate = "25.00";
    break;
  case 3:
    rate = "29.97";
    break;
  default:
    rate = "(please send a bug report)";
    break;
  } 
  mp_msg(0,0," @ %s fps", rate);
}


static void navPrint_PCI_GI(pci_gi_t *pci_gi) {
  int i;

  mp_msg(0,0,"pci_gi:\n");
  mp_msg(0,0,"nv_pck_lbn    0x%08x\n", pci_gi->nv_pck_lbn);
  mp_msg(0,0,"vobu_cat      0x%04x\n", pci_gi->vobu_cat);
  mp_msg(0,0,"vobu_uop_ctl  0x%08x\n", *(uint32_t*)&pci_gi->vobu_uop_ctl);
  mp_msg(0,0,"vobu_s_ptm    0x%08x\n", pci_gi->vobu_s_ptm);
  mp_msg(0,0,"vobu_e_ptm    0x%08x\n", pci_gi->vobu_e_ptm);
  mp_msg(0,0,"vobu_se_e_ptm 0x%08x\n", pci_gi->vobu_se_e_ptm);
  mp_msg(0,0,"e_eltm        ");
  print_time(&pci_gi->e_eltm);
  mp_msg(0,0,"\n");
  
  mp_msg(0,0,"vobu_isrc     \"");
  for(i = 0; i < 32; i++) {
    char c = pci_gi->vobu_isrc[i];
    if((c >= ' ') && (c <= '~'))
      mp_msg(0,0,"%c", c);
    else
      mp_msg(0,0,".");
  }
  mp_msg(0,0,"\"\n");
}

static void navPrint_NSML_AGLI(nsml_agli_t *nsml_agli) {
  int i, j = 0;
  
  for(i = 0; i < 9; i++)
    j |= nsml_agli->nsml_agl_dsta[i];
  if(j == 0)
    return;
  
  mp_msg(0,0,"nsml_agli:\n");
  for(i = 0; i < 9; i++)
    if(nsml_agli->nsml_agl_dsta[i])
      mp_msg(0,0,"nsml_agl_c%d_dsta  0x%08x\n", i + 1, 
	     nsml_agli->nsml_agl_dsta[i]);
}

static void navPrint_HL_GI(hl_gi_t *hl_gi, int *btngr_ns, int *btn_ns) {
  
  if((hl_gi->hli_ss & 0x03) == 0)
    return;
  
  mp_msg(0,0,"hl_gi:\n");
  mp_msg(0,0,"hli_ss        0x%01x\n", hl_gi->hli_ss & 0x03);
  mp_msg(0,0,"hli_s_ptm     0x%08x\n", hl_gi->hli_s_ptm);
  mp_msg(0,0,"hli_e_ptm     0x%08x\n", hl_gi->hli_e_ptm);
  mp_msg(0,0,"btn_se_e_ptm  0x%08x\n", hl_gi->btn_se_e_ptm);

  *btngr_ns = hl_gi->btngr_ns;
  mp_msg(0,0,"btngr_ns      %d\n",  hl_gi->btngr_ns);
  mp_msg(0,0,"btngr%d_dsp_ty    0x%02x\n", 1, hl_gi->btngr1_dsp_ty);
  mp_msg(0,0,"btngr%d_dsp_ty    0x%02x\n", 2, hl_gi->btngr2_dsp_ty);
  mp_msg(0,0,"btngr%d_dsp_ty    0x%02x\n", 3, hl_gi->btngr3_dsp_ty);
  
  mp_msg(0,0,"btn_ofn       %d\n", hl_gi->btn_ofn);
  *btn_ns = hl_gi->btn_ns;
  mp_msg(0,0,"btn_ns        %d\n", hl_gi->btn_ns);
  mp_msg(0,0,"nsl_btn_ns    %d\n", hl_gi->nsl_btn_ns);
  mp_msg(0,0,"fosl_btnn     %d\n", hl_gi->fosl_btnn);
  mp_msg(0,0,"foac_btnn     %d\n", hl_gi->foac_btnn);
}

static void navPrint_BTN_COLIT(btn_colit_t *btn_colit) {
  int i, j;
  
  j = 0;
  for(i = 0; i < 6; i++)
    j |= btn_colit->btn_coli[i/2][i&1];
  if(j == 0)
    return;
  
  mp_msg(0,0,"btn_colit:\n");
  for(i = 0; i < 3; i++)
    for(j = 0; j < 2; j++)
      mp_msg(0,0,"btn_cqoli %d  %s_coli:  %08x\n",
	     i, (j == 0) ? "sl" : "ac",
	     btn_colit->btn_coli[i][j]);
}

static void navPrint_BTNIT(btni_t *btni_table, int btngr_ns, int btn_ns) {
  int i, j;
  
  mp_msg(0,0,"btnit:\n");
  mp_msg(0,0,"btngr_ns: %i\n", btngr_ns);
  mp_msg(0,0,"btn_ns: %i\n", btn_ns);
  
  if(btngr_ns == 0)
    return;
  
  for(i = 0; i < btngr_ns; i++) {
    for(j = 0; j < (36 / btngr_ns); j++) {
      if(j < btn_ns) {
	btni_t *btni = &btni_table[(36 / btngr_ns) * i + j];
	
	mp_msg(0,0,"group %d btni %d:  ", i+1, j+1);
	mp_msg(0,0,"btn_coln %d, auto_action_mode %d\n",
	       btni->btn_coln, btni->auto_action_mode);
	mp_msg(0,0,"coords   (%d, %d) .. (%d, %d)\n",
	       (btni->x_start), (btni->y_start), (btni->x_end), (btni->y_end));
	
	mp_msg(0,0,"up %d, ", btni->up);
	mp_msg(0,0,"down %d, ", btni->down);
	mp_msg(0,0,"left %d, ", btni->left);
	mp_msg(0,0,"right %d\n", btni->right);
	
	// ifoPrint_COMMAND(&btni->cmd);
	mp_msg(0,0,"\n");
      }
    }
  }
}

static void navPrint_HLI(hli_t *hli) {
  int btngr_ns = 0, btn_ns = 0;
  
  mp_msg(0,0,"hli:\n");
  navPrint_HL_GI(&hli->hl_gi, & btngr_ns, & btn_ns);
  navPrint_BTN_COLIT(&hli->btn_colit);
  navPrint_BTNIT(hli->btnit, btngr_ns, btn_ns);
}

void navPrint_PCI(pci_t *pci) {
  mp_msg(0,0,"pci packet:\n");
  navPrint_PCI_GI(&pci->pci_gi);
  navPrint_NSML_AGLI(&pci->nsml_agli);
  navPrint_HLI(&pci->hli);
}

static void navPrint_DSI_GI(dsi_gi_t *dsi_gi) {
  mp_msg(0,0,"dsi_gi:\n");
  mp_msg(0,0,"nv_pck_scr     0x%08x\n", dsi_gi->nv_pck_scr);
  mp_msg(0,0,"nv_pck_lbn     0x%08x\n", dsi_gi->nv_pck_lbn );
  mp_msg(0,0,"vobu_ea        0x%08x\n", dsi_gi->vobu_ea);
  mp_msg(0,0,"vobu_1stref_ea 0x%08x\n", dsi_gi->vobu_1stref_ea);
  mp_msg(0,0,"vobu_2ndref_ea 0x%08x\n", dsi_gi->vobu_2ndref_ea);
  mp_msg(0,0,"vobu_3rdref_ea 0x%08x\n", dsi_gi->vobu_3rdref_ea);
  mp_msg(0,0,"vobu_vob_idn   0x%04x\n", dsi_gi->vobu_vob_idn);
  mp_msg(0,0,"vobu_c_idn     0x%02x\n", dsi_gi->vobu_c_idn);
  mp_msg(0,0,"c_eltm         ");
  print_time(&dsi_gi->c_eltm);
  mp_msg(0,0,"\n");
}

static void navPrint_SML_PBI(sml_pbi_t *sml_pbi) {
  mp_msg(0,0,"sml_pbi:\n");
  mp_msg(0,0,"category 0x%04x\n", sml_pbi->category);
  if(sml_pbi->category & 0x8000)
    mp_msg(0,0,"VOBU is in preunit\n");
  if(sml_pbi->category & 0x4000)
    mp_msg(0,0,"VOBU is in ILVU\n");
  if(sml_pbi->category & 0x2000)
    mp_msg(0,0,"VOBU at the beginning of ILVU\n");
  if(sml_pbi->category & 0x1000)
    mp_msg(0,0,"VOBU at end of PREU of ILVU\n");
  
  mp_msg(0,0,"ilvu_ea       0x%08x\n", sml_pbi->ilvu_ea);
  mp_msg(0,0,"nxt_ilvu_sa   0x%08x\n", sml_pbi->ilvu_sa);
  mp_msg(0,0,"nxt_ilvu_size 0x%04x\n", sml_pbi->size);
  
  mp_msg(0,0,"vob_v_s_s_ptm 0x%08x\n", sml_pbi->vob_v_s_s_ptm);
  mp_msg(0,0,"vob_v_e_e_ptm 0x%08x\n", sml_pbi->vob_v_e_e_ptm);
  
  /* $$$ more code needed here */
}

static void navPrint_SML_AGLI(sml_agli_t *sml_agli) {
  int i;
  mp_msg(0,0,"sml_agli:\n");
  for(i = 0; i < 9; i++) {
    mp_msg(0,0,"agl_c%d address: 0x%08x size 0x%04x\n", i,
	   sml_agli->data[i].address, sml_agli->data[i].size);
  }
}

static void navPrint_VOBU_SRI(vobu_sri_t *vobu_sri) {
  int i;
  int stime[19] = { 240, 120, 60, 20, 15, 14, 13, 12, 11, 
		     10,   9,  8,  7,  6,  5,  4,  3,  2, 1};
  mp_msg(0,0,"vobu_sri:\n");
  mp_msg(0,0,"Next VOBU with Video %08x\n", vobu_sri->next_video);
  for(i = 0; i < 19; i++) {
    mp_msg(0,0,"%3.1f %08x ", stime[i]/2.0, vobu_sri->fwda[i]);
  }
  mp_msg(0,0,"\n");
  mp_msg(0,0,"Next VOBU %08x\n", vobu_sri->next_vobu);
  mp_msg(0,0,"--\n");
  mp_msg(0,0,"Prev VOBU %08x\n", vobu_sri->prev_vobu);
  for(i = 0; i < 19; i++) {
    mp_msg(0,0,"%3.1f %08x ", stime[18 - i]/2.0, vobu_sri->bwda[i]);
  }
  mp_msg(0,0,"\n");
  mp_msg(0,0,"Prev VOBU with Video %08x\n", vobu_sri->prev_video);
}

static void navPrint_SYNCI(synci_t *synci) {
  int i;
  
  mp_msg(0,0,"synci:\n");
  /* $$$ more code needed here */
  for(i = 0; i < 8; i++)
    mp_msg(0,0,"%04x ", synci->a_synca[i]);
  for(i = 0; i < 32; i++)
    mp_msg(0,0,"%08x ", synci->sp_synca[i]);
}

void navPrint_DSI(dsi_t *dsi) {
  mp_msg(0,0,"dsi packet:\n");
  navPrint_DSI_GI(&dsi->dsi_gi);
  navPrint_SML_PBI(&dsi->sml_pbi);
  navPrint_SML_AGLI(&dsi->sml_agli);
  navPrint_VOBU_SRI(&dsi->vobu_sri);
  navPrint_SYNCI(&dsi->synci);
}


