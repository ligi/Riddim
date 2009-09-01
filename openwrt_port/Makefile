# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 9050 2007-09-28 01:23:17Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=riddim
PKG_VERSION:=0.0.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mikrocontroller.cco-ev.de/mikrosvn/Projects/Riddim/
PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=488
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

#PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd

include $(INCLUDE_DIR)/package.mk

define Package/riddim
  SECTION:=utils
  DEPENDS:=+confuse 
  DEPENDS:=+kmod-hid
  CATEGORY:=Base system
  TITLE:=Remote Interactive Digital Drone Interface Mashup
  URL:=http://www.mikrokopter.de/ucwiki/en/Riddim
endef

define Package/riddim/description
 Remote Interactive Digital Drone Interface Mashup
endef



define Package/riddim/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/riddim $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,riddim))

