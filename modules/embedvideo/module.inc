<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2007 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * The implementation of the ItemAddEmbedVideo
 * Supports ItemAddEmbedVideo
 *
 * @package EmbedVideo
 * @author Alan Pippin <apippin@pippins.net>
 * @version $Revision: 1 $
 */
class EmbedVideoModule extends GalleryModule {

    function __construct() {
	global $gallery;
	$this->setId('embedvideo');
	$this->setName($gallery->i18n('Embed Video'));
	$this->setDescription($gallery->i18n('Add embedded videos from the web'));
	$this->setVersion('1.1.5');
	$this->setGroup('import', $gallery->i18n('Import'));
	$this->setCallbacks('getSiteAdminViews');
    $this->setRequiredCoreApi(array(8, 0));
    $this->setRequiredModuleApi(array(4, 0));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations
     */
    function performFactoryRegistrations() {

	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddEmbedVideo', 'ItemAddEmbedVideo',
	    'modules/embedvideo/ItemAddEmbedVideo.inc', 'embedvideo', null);
	if ($ret) {
	    return $ret;
	}

	return null;
    }

    /**
     * @see GalleryModule::getSiteAdminViews
     */
    function getSiteAdminViews() {
	return array(null, array(array('name' => $this->translate($this->getName()),
				       'view' => 'embedvideo.EmbedVideoSiteAdmin')));
    }

    /**
     * @see GalleryModule::upgrade
     */
    function upgrade($currentVersion, $x1=null) {
       global $gallery;

        list ($ret, $params) =
            GalleryCoreApi::fetchAllPluginParameters('module', 'embedvideo');
        if ($ret) {
            return $ret;
        }

        if (!isset($params['embedvideodefaultVariables'])) {
            $ret = $this->setParameter('embedvideodefaultVariables',
				       'height=240'.
				       '|width=320'.
				       '|useInternalFlvPlayer=true'.
				       '|useRemoteSize=false'.
				       '|youtubeShowRelated=false'.
				       '|watermarkVideos=true'
				       );
            if ($ret) {
                return $ret;
            }
        }

        if (!isset($params['embedvideooverrideVariables'])) {
  	    $ret = $this->setParameter('embedvideooverrideVariables', '');
            if ($ret) {
                return $ret;
            }
        }

        return null;
    }
}
?>
