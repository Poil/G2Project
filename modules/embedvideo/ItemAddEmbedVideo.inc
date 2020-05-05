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

GalleryCoreApi::requireOnce('modules/gd/classes/GdToolkitHelper.class');


/**
 * This plugin will handle the addition of embedded video objects
 * @package embedVideo
 * @subpackage UserInterface
 * @author Alan Pippin <apippin@pippins.net>
 * @version $Revision: 1.1 $
 */
class ItemAddEmbedVideo extends ItemAddPlugin {
  
  /**
   * @see ItemAddPlugin::handleRequest
   */
 function handleRequest($form, &$item) {
   global $gallery, $url;
    
   $status = $error = array();
   
   if (isset($form['action']['addEmbedVideoPage'])) {
     
       $platform =& $gallery->getPlatform();

       if (empty($extraHeaders)) {
	   $extraHeaders = array('Referer' => str_replace('&amp;', '&', $url));
       }

       if(isset($form['webPage']['URL'])) {
	 
	   /* Load any stored/set Parameters */
	   list ($ret, $params) =
	     GalleryCoreApi::fetchAllPluginParameters('module', 'embedvideo');
	   if ($ret) {
	       return array($ret, null, null);
	   }
	   foreach (array('default', 'override') as $type) {
	       $ItemAddUploadApplet[$type] = array();
	       if (!empty($params['embedvideo' . $type . 'Variables'])) {
		   $variablesArray = explode('|', $params['embedvideo' . $type . 'Variables']);
		   foreach ($variablesArray as $variable) {
		       list ($name, $value) = explode('=', $variable);
		       $ItemAddEmbedVideo[$type][$name] = $value;
		       /* print "type: $type name: $name value: $value <br>"; */
		   }
	       }
	   }
	   
	   /* Store any Parameters into some simpler, shorter, local variables */
	   global $debugOutput, $useInternalFlvPlayer;
	   global $width, $height, $externalFlvPlayer, $externalFlvPlayerVars;
	   global $flvThumbnail, $useRemoteSize, $autoStart;
	   global $watermarkVideos, $watermarkImage, $watermarkAlignment;
	   global $unsupported_url;

	   /* Find out what value our parameters should have by looking to see if they
            * are defined in our overrides section or default section. If they are not
            * defined in either of these 2 places, pass in a default value to set them to
	    */
	   $debugOutput = $this->getParameter($ItemAddEmbedVideo, 'debugOutput', "false");
	   $useInternalFlvPlayer = $this->getParameter($ItemAddEmbedVideo, 'useInternalFlvPlayer', "true");
	   $youtubeShowRelated = $this->getParameter($ItemAddEmbedVideo, 'youtubeShowRelated', "false");
	   $width = $this->getParameter($ItemAddEmbedVideo, 'width', "320");
	   $height = $this->getParameter($ItemAddEmbedVideo, 'height', "240");
	   $externalFlvPlayer = $this->getParameter($ItemAddEmbedVideo, 'externalFlvPlayer', "");
	   $externalFlvPlayerVars = $this->getParameter($ItemAddEmbedVideo, 'externalFlvPlayerVars', "");
	   $flvThumbnail = $this->getParameter($ItemAddEmbedVideo, 'flvThumbnail', "");
	   $useRemoteSize = $this->getParameter($ItemAddEmbedVideo, 'useRemoteSize', "false");
	   $autoStart = $this->getParameter($ItemAddEmbedVideo, 'autoStart', "false");
	   $watermarkImage = $this->getParameter($ItemAddEmbedVideo, 'watermarkImage', "");
	   $watermarkVideos = $this->getParameter($ItemAddEmbedVideo, 'watermarkVideos', "false");
	   $watermarkAlignment = $this->getParameter($ItemAddEmbedVideo, 'watermarkAlignment', "left");
	   $wordwrapSummary = $this->getParameter($ItemAddEmbedVideo, 'wordwrapSummary', "0");
	   $wordwrapDescription = $this->getParameter($ItemAddEmbedVideo, 'wordwrapDescription', "0");
           $allowFullScreen = $this->getParameter($ItemAddEmbedVideo, 'allowFullScreen', "false");
	   $centerVideo = $this->getParameter($ItemAddEmbedVideo, 'centerVideo', "false");
	   
	   /* Print our stored/set Parameters */
	   if(!strcmp($debugOutput,"true")) {
	     print "\n<hr>";
	     print "<h2>Variable Parameters</h2>\n";
	     print "debugOutput=$debugOutput<br>\n";
	     print "useInternalFlvPlayer=$useInternalFlvPlayer<br>\n";
	     print "youtubeShowRelated=$youtubeShowRelated<br>\n";
	     print "width=$width<br>\n";
	     print "height=$height<br>\n";
	     print "externalFlvPlayer=$externalFlvPlayer<br>\n";
	     print "externalFlvPlayerVars=$externalFlvPlayerVars<br>\n";
	     print "flvThumbnail=$flvThumbnail<br>\n";
	     print "useRemoteSize=$useRemoteSize<br>\n";
	     print "autoStart=$autoStart<br>\n";
	     print "watermarkVideos=$watermarkVideos<br>\n";
	     print "watermarkImage=$watermarkImage<br>\n";
	     print "watermarkAlignment=$watermarkAlignment<br>\n";
	     print "wordwrapSummary=$wordwrapSummary<br>\n";
	     print "wordwrapDescription=$wordwrapDescription<br>\n";
	     print "allowFullScreen=$allowFullScreen<br>\n";
	     print "centerVideo=$centerVideo\n";
	     print "<hr>\n";
	   }

	   /* Store other string constants we'll use later */
	  
	   /* youtube */
	   $youtubeUrlPattern="youtube";
	   $youtubeApiUrl="http://gdata.youtube.com/feeds/api/";
	   $youtubeThumbnailUrl="http://img.youtube.com/vi/";
	   /* We can't extract the server size from youtube */
	   $youtubeWidth="425";
	   $youtubeHeight="350";

	   /* google */
	   $googleUrlPattern="video.google";
	   $googlePlayer="http://video.google.com/googleplayer.swf";
	   /* We can't extract the server size from google video */
	   $googleWidth="400";
	   $googleHeight="326";
	   
	   /* yahoo */
	   $yahooUrlPattern="video.yahoo";
	   $yahooThumbnailUrl="http://thmg01.video.search.yahoo.com/image/";
	   
	   /* metacafe */
	   $metacafeUrlPattern="metacafe";
	   $metacafeThumbnailUrl="http://www.metacafe.com/thumb/";

	   /* revver */
	   $revverUrlPattern="revver";
	   $revverThumbnailUrl="http://frame.revver.com/frame/120x90/";

	   /* dailymotion */
	   $dailymotionUrlPattern="dailymotion";
	   $dailymotionThumbnailUrl="http://www.dailymotion.com/thumbnail/160x120/video/";

	   /* divshare */
	   $divshareUrlPattern="divshare";
	   $divshareThumbnailUrl="http://www.divshare.com/direct/video_thumb/";

	   /* stickam */
	   $stickamUrlPattern="stickam";
	   $stickamThumbnailUrl="http://static.stickam.com/";

	   /* myspace */
	   $myspaceUrlPattern="vids.myspace";
	   $myspaceUrlPattern2="myspacetv";
	   $myspaceThumbnailUrl="http://mediaservices.myspace.com/services/rss.ashx?type=video&videoID=";

	   /* vimeo */
	   $vimeoUrlPattern="vimeo.com";

	   /* animoto */
	   $animotoUrlPattern="animoto.com";
	   
	   /* Gallery2 specific paths and variables */
	   $urlGenerator =& $gallery->getUrlGenerator();
	   $gallery2_url = $urlGenerator->getCurrentUrlDir();
	   $gallery2_flv_thumbnail = "modules/embedvideo/images/G2video_thumbnail.jpg";
	   $gallery2_video_watermark = "modules/embedvideo/images/G2video_watermark1.png";
	   $gallery2_flv_player = "modules/flashvideo/lib/G2flv.swf";

	   /* Unsupported URLs */
	   $unsupported_url=0;
	   
	   /* Store the passed URL in a shorter local variable */
	   /* Strip trailing whitespace in the url, this will cause problems later */
	   $url = $form['webPage']['URL'];
           if(preg_match("/(.*?)\s+/",$url,$matches)) {
	     $url = $matches[1];
	   }
	   
	   /*
	    *****************************
	    * Embed a Youtube Video
	    *****************************
	    */
	   if(preg_match("/$youtubeUrlPattern/",$url)) {
	     
 	       /* Make sure we can find a video_id in the URL */
  	       $extra_params = "";
	       if(preg_match("/watch\?v=(.*?)(&\S+=\S+)/",$url,$matches)) {
		   $video_id = $matches[1];
		   $extra_params = $matches[2];
	       } else if (preg_match("/watch\?v=(.*)/",$url,$matches)) {
		   $video_id = $matches[1];
	       } else if (preg_match("/v\/(.*)/",$url,$matches)) {
		   $video_id = $matches[1];
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract video id from url: $url"),null,null);
	       }

	       /* If extra params were given, handle them here */
	       if(strcmp($extra_params,"")) {
		 /* Handle the high quality format information */
		 if(preg_match("/fmt=(\d+)/",$extra_params,$matches)) {
		   $extra_params="&ap=%2526fmt%3D".$matches[1];
		 }
	       }
	       	       
	       /* Youtube api feed */
	       $feed = $youtubeApiUrl."videos/$video_id";

	       if(!strcmp($debugOutput,"true")) {
		   print "\n<h2>Youtube URL Parsing Results</h2>";
 		   print "video_id: $video_id<br>";
		   print "extra_embed_params: $extra_params<br>";
		   print "gdata feed: $feed<br>";
                   print "<hr>";
	       }

	       /* Get the youtube xml feed as a string data source */
	       list ($successfullyCopied, $xml, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($feed, $extraHeaders);
	       if (!$successfullyCopied) {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
						    "Unable to get video information at url: $url - $response"),null,null);
	       }
		 
	       if(preg_match("/This video is private/",$xml)) {
		 return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
						    "Unable to retrieve video information from youtube.".
						    "You cannot embed a private youtube video: $url"),null,null);
	       }
		 
	       if(!strcmp($debugOutput,"true")) {
		 print "\n<h2>Youtube XML Dump</h2>";
		 print "$xml";
		 print "<hr>";
	       }
		 
	       /* Extract certain information from the xml feed */
	       preg_match_all("/\<media:title.+?\>(.+?)\<\/media:title\>/smi",$xml, $title);
	       preg_match_all("/\<media:description.+?\>(.+?)\<\/media:description\>/smi",$xml, $description);
	       preg_match_all("/\<media:thumbnail url='(.+?)'/smi",$xml, $thumbnail);
		 
	       array_shift($title);
	       array_shift($thumbnail);
	       array_shift($description);

	       /* Replace html characters. More can be added but this seems to work */
	       for($i=0;$i<count($description[0]);$i++){	  
		 $description[0][$i] = preg_replace("/&#60;/","<",$description[0][$i]);
		 $description[0][$i] = html_entity_decode($description[0][$i],ENT_QUOTES);	  
	       }
		 
	       /* Store the information found in some local variables */
	       $title = $title[0][0];
	       $summary = $description[0][0];
		 
	       /* The last thumbnail match we make will always be the highest resolution */
	       $thumbnail = $thumbnail[0][sizeof($thumbnail[0])-1];

	       if(!strcmp($debugOutput,"true")) {
		 print "title: $title<br>";
		 print "summary: $summary<br>";
		 print "thumbnail: $thumbnail<br>";
	       }
		 
	       /* Determine what our width and height should be based on our useRemoteSize parameter */
	       if(!strcmp($useRemoteSize,"true")) {
		 $width = $youtubeWidth;
		 $height = $youtubeHeight;
	       }
		 
	       /* Determine if the video should autoplay or not based on the autoStart parameter */
	       $autoStartStr="";
	       if(!strcmp($autoStart,"true")) {
		 $autoStartStr="&autoplay=1";
	       }
		 
	       /* Determine if the video should show related videos or not based on the youtubeShowRelated parameter */
	       $youtubeShowRelatedStr="";
	       if(!strcmp($youtubeShowRelated,"false")) {
		 $youtubeShowRelatedStr="&rel=0";
	       }

	       /* Determine if the video should show related videos or not based on the youtubeShowRelated parameter */
	       $fullScreenStr="";
	       $fullScreenParamStr1="";
	       $fullScreenParamStr2="";
	       if(!strcmp($allowFullScreen,"true")) {
		 $fullScreenStr="&fs=1";
		 $fullScreenParamStr1="<param name=\"allowFullScreen\" value=\"true\"></param>";
		 $fullScreenParamStr2=" allowfullscreen=\"true\"";
	       }

	       /* Format the description to hold a reference to the embedded video */
	       $description = '<object width="'.$width.'" height="'.$height.'">';
	       $description.= '<param name="movie" ';
	       $description.= 'value="http://www.youtube.com/v/'.$video_id.$extra_params.'"></param>';
	       $description.= '<param name="wmode" value="transparent"></param>';
	       $description.= $fullScreenParamStr1;
	       $description.= '<embed src="http://www.youtube.com/v/';
	       $description.= $video_id.$extra_params.$autoStartStr.$youtubeShowRelatedStr.$fullScreenStr;
	       $description.= '" type="application/x-shockwave-flash" wmode="transparent" ';
	       $description.= 'width="'.$width.'" height="'.$height.'"'.$fullScreenParamStr2.'></embed></object>';

	   /*
	    **********************************
	    * Embed a Google Video
	    **********************************
            */
	   } else if(preg_match("/$googleUrlPattern/",$url)) {

	       /* Make sure we can extract a docID */
	       if(preg_match("/docid=(.*)/",$url,$matches)) {
		   $doc_id = $matches[1];
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract doc id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video on video.google.com */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       //print "contents: $contents <br>";
	       preg_match('/<meta name="description" content="(.+?)\. \w+ \d+, \d+.*">/i',
			  $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       $title="Unknown";
	       if(preg_match('/<title>(.+?)\s+- Google Video<\/title>/i', $contents, $matches)) {
		 $title=$matches[1];
	       } else if(preg_match('/<title>(.+?)<\/title>/i', $contents, $matches)) {
		 $title=$matches[1];
	       }

	       /* Extract the thumbnail URL from the webpage contents */
	       preg_match('/\'.+?thumbnail.+?(http:\/\/.*?\/ThumbnailServer2.+?)\'/i',
			  $contents, $matches);
	       $thumbnail=$matches[1];
	       $thumbnail=preg_replace("/\\\\x26/","&",$thumbnail);
	       $thumbnail=preg_replace("/\\\\x3d/","=",$thumbnail);
	       $thumbnail=urldecode($thumbnail);
	       
	       /* Determine what our width and height should be based on our useRemoteSize parameter */
	       if(!strcmp($useRemoteSize,"true")) {
		 $width = $googleWidth;
		 $height = $googleHeight;
	       }

	       /* Determine if the video should autoplay or not based on the autoStart parameter */
	       $autoStartStr="";
	       if(!strcmp($autoStart,"true")) {
		 $autoStartStr="&autoplay=1";
	       }

	       /* Format the description to hold a reference to the embedded video */
	       $description = '<embed ';
	       $description.= 'style="width:'.$width.'px; height:'.$height.'px;" id="VideoPlayback" ';
	       $description.= 'type="application/x-shockwave-flash" ';
	       $description.= 'src="'.$googlePlayer.'?docId='.$doc_id.$autoStartStr.'" flashvars=""> ';
	       $description.= '</embed>';

	   /*
	    **********************************
	    * Embed a Yahoo Video
	    **********************************
            * TODO: The autoStart parameter doesn't work with the yahoo video player
            */
	   } else if(preg_match("/$yahooUrlPattern/",$url)) {

	       /* Make sure we can extract a vidID */
	       if(preg_match("/vid=(.*)/",$url,$matches)) {
		   $vid_id = $matches[1];
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract vid id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       preg_match('/Description:<\/em><p>(.+?)<\/p>/', $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       $title="Unknown";
	       if(preg_match('/<title>(.+?)\s+- Yahoo! Video<\/title>/i', $contents, $matches)) {
		 $title=$matches[1];
	       } else if(preg_match('/<title>(.+?)<\/title>/i', $contents, $matches)) {
		 $title=$matches[1];
	       }

	       /* Build the thumbnail URL from the vid_id */
	       $thumbnail=$yahooThumbnailUrl.$vid_id."_01";

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/(<embed src.+?<\/embed>)/', $contents, $matches)) { 
		 $description=$matches[1];
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width='\d+'/","width='".$width."'",$description);
		   $description=preg_replace("/height='\d+'/","height='".$height."'",$description);
		 }
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }

	   /*
	    **********************************
	    * Embed a MetaCafe Video
	    **********************************
            */
	   } else if(preg_match("/$metacafeUrlPattern/",$url)) {

	       /* Make sure we can extract a itemID */
	       if(preg_match("/\/watch\/(.+?)\/(.+?)/",$url,$matches)) {
		   $item_id = $matches[1];
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract item id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       preg_match('/<meta name="description" content="(.+?)" \/>/i', $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       preg_match('/<title>(.+?)<\/title>/i', $contents, $matches);
	       $title=$matches[1];
	       
	       /* Build the thumbnail URL from the item_id */
	       $thumbnail=$metacafeThumbnailUrl.$item_id.".jpg";

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/(embed src.+?\/embed)/', $contents, $matches)) { 
		 $description="<".$matches[1];
		 $description=preg_replace("/&quot;/","'",$description);
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width='\d+'/","width='".$width."'",$description);
		   $description=preg_replace("/height='\d+'/","height='".$height."'",$description);
		 }
		 if(!strcmp($autoStart,"true")) {
		   $description=preg_replace("/\.swf/",".swf?playerVars=autoPlay=yes",$description);
		 }
		 $description.= "</embed>";
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }

           /*
	    **********************************
	    * Embed a Revver Video
	    **********************************
            */
	   } else if(preg_match("/$revverUrlPattern/",$url)) {

	       /* Make sure we can extract a itemID */
	       if(preg_match("/\/watch\/(.+)/",$url,$matches)) {
		   $item_id = $matches[1];
		   $item_id=preg_replace("/\/$/","",$item_id);
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract item id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       if(preg_match('/"video_description">(.+?)<\/p>/i', $contents, $matches)) {
		 $summary=$matches[1];
	       }
	       if(preg_match('/"video_description">(.+)/i', $contents, $matches)) {
		 $summary=$matches[1];
	       }

	       /* Extract the title from the webpage contents */
	       preg_match('/digg_title = \'(.+?)\'/i', $contents, $matches);
	       $title=$matches[1];
	       
	       /* Build the thumbnail URL from the item_id */
	       $thumbnail=$revverThumbnailUrl.$item_id.".jpg";

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/<input class.+?(script src.+?\/script)/', $contents, $matches)) {
		 $description="<".$matches[1];
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width:\d+/","width:".$width,$description);
		   $description=preg_replace("/height:\d+/","height:".$height,$description);
		 }
		 if(!strcmp($autoStart,"true")) {
		   preg_match('/(height:\d+)/',$description,$matches);
		   $height=$matches[1];
		   $description=preg_replace("/$height/",$height.";autoplay:true",$description);
		 }
		 $description.= ">";
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }
	    
	   /*
	    **********************************
	    * Embed a DailyMotion Video
	    **********************************
            */
	   } else if(preg_match("/$dailymotionUrlPattern/",$url)) {
	     
	       /* Make sure we can extract a itemID */
	       if(preg_match("/\/video\/(.+)/",$url,$matches)) {
		   $item_id = $matches[1];
		   $item_id=preg_replace("/\/$/","",$item_id);
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract item id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       preg_match('/<div class="description\s+foreground">(.+?)<\/div>/i', $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       preg_match('/<meta name="title" content="Dailymotion : (.+?)" \/>/i', $contents, $matches);
	       $title=htmlentities($matches[1], ENT_QUOTES, "UTF-8");
	       
	       /* Build the thumbnail URL from the item_id */
	       $thumbnail=$dailymotionThumbnailUrl.$item_id;

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/<.+? value="(.+?)" class=".+?" id="video_player_embed_code_text" \/>/', $contents, $matches)) { 
		 $description=$matches[1];
		 $description=preg_replace("/&quot;/","'",$description);
		 $description=preg_replace("/&lt;/","<",$description);
		 $description=preg_replace("/&gt;/",">",$description);
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width=\"\d+\"/","width=\"".$width."\"",$description);
		   $description=preg_replace("/height=\"\d+\"/","height=\"".$height."\"",$description);
		 }
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }
	       
	   /*
	    **********************************
	    * Embed a DivShare Video
	    **********************************
            */
	   } else if(preg_match("/$divshareUrlPattern/",$url)) {
	     
	       /* Make sure we can extract a itemID */
	       if(preg_match("/\/download\/(.+)/",$url,$matches)) {
		   $item_id = $matches[1];
		   $item_id=preg_replace("/\/$/","",$item_id);
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract item id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       //preg_match('/<meta name="description" content="(.+?)" \/>/i', $contents, $matches);
	       //$summary=$matches[1];
	       // Right now, the description is a plug for divshare, nothing user specific about it
	       $summary="";

	       /* Extract the title from the webpage contents */
	       preg_match('/<title>(.+?) - Divshare<\/title>/i', $contents, $matches);
	       $title=$matches[1];
	       
	       /* Build the thumbnail URL from the item_id */
	       $thumbnail=$divshareThumbnailUrl.$item_id;

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/(<embed src.+?<\/embed>)/', $contents, $matches)) { 
		 $description=$matches[1];
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width=\"\d+\"/","width=\"".$width."\"",$description);
		   $description=preg_replace("/height=\"\d+\"/","height=\"".$height."\"",$description);
		 }
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }

	   /*
	    **********************************
	    * Embed a StickAm Video
	    **********************************
            */
	   } else if(preg_match("/$stickamUrlPattern/",$url)) {

	       /* Make sure we can extract a itemID */
	       if(preg_match("/mId=(.+)/",$url,$matches)) {
		   $item_id = $matches[1];
		   $item_id=preg_replace("/\/$/","",$item_id);
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract item id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       $url=preg_replace("/&amp;/","&",$url);
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       preg_match('/<meta name="description" content="(.+?)"\/>/i', $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       preg_match('/<meta name="title" content="(.+?)">/i', $contents, $matches);
	       $title=$matches[1];	       
	       
	       /* Build the thumbnail URL from the item_id */
	       if(preg_match('/movieName=(.+?)&userId=/i', $contents, $matches)) { 
		 $thumbnail_url=$matches[1];
		 $thumbnail_url=preg_replace("/%2F/","/",$thumbnail_url);
		 $thumbnail=$stickamThumbnailUrl.$thumbnail_url.".jpg";
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract thumbnail from url: $url"),NULL,NULL);
	       }

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/readonly value="(.+?)"/', $contents, $matches)) { 
		 $description=$matches[1];
		 $description=preg_replace("/&quot;/","'",$description);
		 $description=preg_replace("/&lt;/","<",$description);
		 $description=preg_replace("/&gt;/",">",$description);
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width='\d+'/","width='".$width."'",$description);
		   $description=preg_replace("/height='\d+'/","height='".$height."'",$description);
		 }
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }
	     
	   /*
	    **********************************
	    * Embed a Myspace Video
	    **********************************
            */
	   } else if(preg_match("/($myspaceUrlPattern|$myspaceUrlPattern2)/",$url)) {

	       /* Make sure we can extract a itemID */
	       if(preg_match("/videoid=(.+)/i",$url,$matches)) {
		   $video_id = $matches[1];
		   $video_id=preg_replace("/\/$/","",$video_id);
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract video id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       preg_match('/class="description" id="vid_description">(.+?)<\/div>/i', $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       preg_match('/<h1>(.+?)<\/h1>/i', $contents, $matches);
	       $title=$matches[1];	       
	       
	       /* Build the thumbnail URL from the item_id */
	       $video_info_url=$myspaceThumbnailUrl.$video_id;
	       list ($successfullyCopied, $video_info, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($video_info_url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }
	       if(preg_match('/thumbnail url="(.+?)"/',$video_info, $matches)) {
		 $thumbnail = $matches[1];
	       } else {
		  /*
		   * Set the thumbnail to some generic jpg image,
		   * since we can't extract it from the site.
		   * If no parameter is set, set it to a default value.
		   */
		 if(preg_match("/\w+/", $flvThumbnail)) {
		   $thumbnail = $flvThumbnail;
		 } else {
		   $thumbnail = $gallery2_url.$gallery2_flv_thumbnail;
		 }
	       }

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/(&lt;embed src.+?\/embed&gt;)/', $contents, $matches) ||
		  preg_match('/(&lt;embed src.+?\/object&gt;)/', $contents, $matches)) { 
		 $description=$matches[1];
		 $description=preg_replace("/&quot;/","'",$description);
		 $description=preg_replace("/&lt;/","<",$description);
		 $description=preg_replace("/&gt;/",">",$description);
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width='\d+'/","width='".$width."'",$description);
		   $description=preg_replace("/height='\d+'/","height='".$height."'",$description);
		 }
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }

	   /*
	    **********************************
	    * Embed a Vimeo Video
	    **********************************
            */
	   } else if(preg_match("/$vimeoUrlPattern/",$url)) {

	       /* Make sure we can extract a docID */
	       if(preg_match("/$vimeoUrlPattern\/(.*)/",$url,$matches)) {
		   $video_id = $matches[1];
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract video id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the summary from the webpage contents */
	       preg_match('/<meta name="description" content="(.+?)"/i',
			  $contents, $matches);
	       $summary=$matches[1];

	       /* Extract the title from the webpage contents */
	       preg_match('/<meta name="title" content="(.+?)"/i',
			  $contents, $matches);
	       $title=$matches[1];

	       /* Extract the thumbnail URL from the webpage contents */
	       preg_match('/"videothumbnail" href="(.+?)"/i',
			  $contents, $matches);
	       $thumbnail=$matches[1];

	       /* Determine what our width and height should be based on our useRemoteSize parameter */
	       if(!strcmp($useRemoteSize,"true")) {
		 preg_match('/id="vimeo_player_'.$video_id.'".+?style="width:(\d+)px;height:(\d+)px;"/i',
			    $contents, $matches);
		 $width = $matches[1];
		 $height = $matches[2];
	       }

	       /* Determine if the video should autoplay or not based on the autoStart parameter */
	       $autoStartStr="";
	       if(!strcmp($autoStart,"true")) {
		 $autoStartStr="&amp;autoplay=1";
	       }

	       /* Format the description to hold a reference to the embedded video */
	       $description = '<object width="'.$width.'" height="'.$height.'">';
	       $description.= '<param name="allowfullscreen" value="true" />';
	       $description.= '<param name="allowscriptaccess" value="always" />';
	       $description.= '<param name="movie" value="http://vimeo.com/moogaloop.swf?clip_id='.$video_id;
	       $description.= '&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;'.$autoStartStr;
	       $description.= 'show_portrait=0&amp;color=&amp;fullscreen=1"/ >';
	       $description.= '<embed src="http://vimeo.com/moogaloop.swf?clip_id='.$video_id;
	       $description.= '&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;'.$autoStartStr;
	       $description.= 'show_portrait=0&amp;color=&amp;fullscreen=1"';
	       $description.= 'type="application/x-shockwave-flash" allowfullscreen="true" allowscriptaccess="always"';
	       $description.= 'width="'.$width.'" height="'.$height.'">';
	       $description.= '</embed></object>';

	   /*
	    **********************************
	    * Embed an Animoto Video
	    **********************************
            */
	   } else if(preg_match("/$animotoUrlPattern/",$url)) {
	     
	       /* Make sure we can extract a itemID */
	       if(preg_match("/\/play\/(.+)/",$url,$matches)) {
		   $item_id = $matches[1];
		   $item_id=preg_replace("/\/$/","",$item_id);
	       } else {
		   return array(GalleryCoreApi::error(ERROR_BAD_PARAMETER,__FILE__,__LINE__,
				"Unable to extract item id from url: $url"),null,null);
	       }

	       /* Grab the contents of the webpage used to display the video */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $url - $response"),NULL,NULL);
	       }

	       /* Extract the embed video xml page from the webpage contents */
	       if(preg_match('/type="text\/xml\+oembed" href="(.+?)"/', $contents, $matches)) { 
		 $xml_url=$matches[1];
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
						    "Unable to extract xml information from url: $url"),NULL,NULL);
	       }

	       /* Get the xml page contents */
	       list ($successfullyCopied, $contents, $response, $headers) =
		 GalleryCoreApi::fetchWebPage($xml_url, $extraHeaders);
	       if (!$successfullyCopied) {
		   return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
				"Unable to get video information at url: $xml_url - $response"),NULL,NULL);
	       }

	       /* Extract the title from the webpage contents */
	       preg_match('/<title>(.+?)<\/title>/i', $contents, $matches);
	       $title=htmlentities($matches[1], ENT_QUOTES, "UTF-8");
	       
	       /* Extract the thumbnail from the webpage contents */
	       preg_match('/<thumbnail_url>(.+?)<\/thumbnail_url>/i', $contents, $matches);
	       $thumbnail=$matches[1];

	       /* Format the description to hold a reference to the embedded video */
	       if(preg_match('/<html>(.+?)<\/html>/ism', $contents, $matches)) { 
		 $description=$matches[1];
		 $description=preg_replace("/&quot;/","'",$description);
		 $description=preg_replace("/&lt;/","<",$description);
		 $description=preg_replace("/&gth;/",">",$description);
		 $description=preg_replace("/\\\\\"/","\"",$description);
		 if(!strcmp($useRemoteSize,"false")) {
		   $description=preg_replace("/width=\"\d+\"/","width=\"".$width."\"",$description);
		   $description=preg_replace("/height=\"\d+\"/","height=\"".$height."\"",$description);
		 }
	       } else {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to extract embedded video information from url: $url"),NULL,NULL);
	       }
	   /*
	    **********************************
	    * Embed a remote .swf file
	    **********************************
            */
	   } else if(preg_match("/.*\/(.+?)\.swf/i",$url,$matches)) {

	       /* Set the title and summary to the name of the file */
	       $title = $matches[1];
	       $summary = $matches[1];
	       
	       /*
                * Set the thumbnail to some generic jpg image,
	        * since we can't extract it from the remote swf file.
	        * If no parameter is set, set it to a default value.
                */
	       if(preg_match("/\w+/", $flvThumbnail)) {
		 $thumbnail = $flvThumbnail;
	       } else {
		 $thumbnail = $gallery2_url.$gallery2_flv_thumbnail;
	       }

	       /*
                * Check to make sure the URL to the remote swf file is valid
	        * (That the file exists at the URL given)
                */
	       list ($successfullyCopied, $response, $headers) =
		 $this->fetchWebFileHeaders($url, $extraHeaders);
	       if (!$successfullyCopied) {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			      "Unable to find the video at url: $url - $response"),NULL,NULL);
	       }
	       
	       /*
                * Format the description to hold a reference to the embedded video
                */

	       /* Format the description to hold a reference to the embedded video */
	       $description ='<embed src="'.$url.'">';
	       
	   /*
	    **********************************
	    * Embed a remote .flv or .mp4 file
	    **********************************
            */
	   } else if(preg_match("/.*\/(.+?)\.flv/i",$url,$matches) ||
		     preg_match("/.*\/(.+?)\.mp4/i",$url,$matches)) {
	     
	       /* Set the title and summary to the name of the file */
	       $title = $matches[1];
	       $summary = $matches[1];
	       
	       /*
                * Set the thumbnail to some generic jpg image,
	        * since we can't extract it from the remote flv file.
	        * If no parameter is set, set it to a default value.
                */
	       if(preg_match("/\w+/", $flvThumbnail)) {
		 $thumbnail = $flvThumbnail;
	       } else {
		 $thumbnail = $gallery2_url.$gallery2_flv_thumbnail;
	       }

	       /*
                * Check to make sure the URL to the remote flv file is valid
	        * (That the file exists at the URL given)
                */
	       list ($successfullyCopied, $response, $headers) =
		 $this->fetchWebFileHeaders($url, $extraHeaders);
	       if (!$successfullyCopied) {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			      "Unable to find the video at url: $url - $response"),NULL,NULL);
	       }
	       
	       /*
                * Format the description to hold a reference to the embedded video
		* This reference will be embedded using the G2 internal player,
	        * or an external player if provided by the user.
                */
	       if(!strcmp($useInternalFlvPlayer,"false")) {

		   /*
                    * The user has indicated they want to use an external flv player
		    * Make sure one is defined!
                    */
		   if(!preg_match("/\w+/",$externalFlvPlayer)) {
		       return array(GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED,__FILE__,__LINE__,
				   "Invalid/missing external player parameter"),null,null);
		   }

		   /* Format the description to hold a reference to the embedded video */
		   $description ='<embed src="'.$externalFlvPlayer.'" ';
		   $description.= 'width="'.$width.'" height="'.$height.'" ';
		   $description.= 'bgcolor="#C0C0C0" allowfullscreen="true" ';
		   $description.= 'type="application/x-shockwave-flash" ';
		   $description.= 'pluginspage="http://www.macromedia.com/go/getflashplayer" ';
		   $description.= 'flashvars="file='.$url;
		   $description.= '&fullscreenpage='.$thumbnail;
		   $description.= '&linktarget=_Blank&image='.$thumbnail;

		   if(!preg_match("/\w+/",$externalFlvPlayerVars)) {
		       /* Format the flashvars for the internal G2 flv player */
		       $description.= '&showdigits=true&autostart='.$autoStart.'&showfsbutton=true&';
  		       $description.= '&repeat=false&lightcolor=0x9999FF';
		       $description.= '&backcolor=0x888888&frontcolor=0x000000"';
		   } else {
		       /* Format the flashvars for the external G2 flv player */
		       $description.= '&'.$externalFlvPlayerVars;
		   }
		   $description.=  ' />&nbsp;</p>';
	       
	       /* Internal FLV player */
	       } else {
		   /* Format the description to hold a reference to the embedded video */
		   $macromedia_url = "http://download.macromedia.com/pub/shockwave/cabs/flash/";
		   $description = '<script type="text/javascript">'."\n";
		   $description.= '// <![CDATA['."\n";
		   $description.= 'function divResize(id, nw, nh) {'."\n";
		   $description.= 'var obj = document.getElementById(id);'."\n";
		   $description.= 'obj.style.width = nw + "px";'."\n";
		   $description.= 'obj.style.height = nh + "px";'."\n";
		   $description.= '}'."\n";
		   $description.= '// ]]>'."\n";
		   $description.= '</script>'."\n";
		   $description.= '<div id="flashvideo" style="align:left;width:525px;height:392px">'."\n";
		   $description.= '<object classid="clsid:D27CDB6E-AE6D-11CF-96B8-444553540000"';
		   $description.= 'codebase="'.$macromedia_url.'swflash.cab#version=8,0,0,0"';
		   $description.= 'width="100%" height="100%" id="IFid1" class="ImageFrame_image">';
		   $description.= '<param name="movie" value="'.$gallery2_url.$gallery2_flv_player.'"/>';
		   $description.= '<param name="FlashVars" value="flvUrl='.$url;
		   $description.= '&Width='.$width.'&Height='.$height.'&title='.$title;
		   $description.= '&allowDl=true&thumbUrl='.$thumbnail.'&autoStart='.$autoStart;
		   $description.= '&langDownload=Download&langLarge=Large&langNormal=Normal"/>';
		   $description.= '<param name="quality" value="high"/>';
		   $description.= '<param name="scale" value="noscale"/>';
		   $description.= '<param name="salign" value="lt"/>';
		   $description.= '<param name="wmode" value="transparent"/>';
		   $description.= '<param name="allowScriptAccess" value="always"/>';
		   $description.= '<embed src="'.$gallery2_url.$gallery2_flv_player.'" ';
		   $description.= 'flashvars="flvUrl='.$url;
		   $description.= '&Width='.$width.'&Height='.$height.'&title='.$title;
		   $description.= '&allowDl=true&thumbUrl='.$thumbnail.'&autoStart='.$autoStart;
		   $description.= '&langDownload=Download&langLarge=Large&langNormal=Normal" ';
		   $description.= 'type="application/x-shockwave-flash" ';
		   $description.= 'width="100%" height="100%" quality="high" scale="noscale" salign="lt" ';
		   $description.= 'wmode="transparent" allowScriptAccess="always" ';
		   $description.= 'pluginspage="http://www.macromedia.com/go/getflashplayer"/>';
		   $description.= '</object></div>';
	       }

	   /*
	    **********************************
	    * Embed a generic <object ... <embed ... video
	    **********************************
            */ 
	   } else {

	     /* Grab the contents of the webpage used to display the video */
	     list ($successfullyCopied, $contents, $response, $headers) =
	       GalleryCoreApi::fetchWebPage($url, $extraHeaders);
	     if (!$successfullyCopied) {
	       return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
						  "Unable to get video information at url: $url - $response"),NULL,NULL);
	     }

	     /* Extract the URL base (site name) */
	     preg_match("/(http:\/\/.+?)\//i",$url,$matches);
	     $site = $matches[1];
	     print "site: $site<br>";
	     
	     /* Extract the summary from the webpage contents */
	     $summary="Unknown";
	     if(preg_match('/<meta name="description" content="(.+?)"/si', $contents, $matches)) {
	       $summary=$matches[1];
	     }

	     /* Extract the title from the webpage contents */
	     $title="Unknown";
	     if(preg_match('/<title>(.+?)<\/title>/i', $contents, $matches)) {
	       $title=$matches[1];
	     }

	     /*
	      * Set the thumbnail to some generic jpg image,
	      * since we can't extract it from a generic website.
	      * If no parameter is set, set it to a default value.
	      */
	     if(preg_match("/\w+/", $flvThumbnail)) {
	       $thumbnail = $flvThumbnail;
	     } else {
	       $thumbnail = $gallery2_url.$gallery2_flv_thumbnail;
	     }

	     /* Format the description to hold a reference to the embedded video */
	     if(preg_match('/(<object.+?<embed.+?<\/object>)/', $contents, $matches)) { 
	       $description=$matches[1];
	       if(!strcmp($useRemoteSize,"false")) {
		 $description=preg_replace("/width=\"\d+\"/","width=\"".$width."\"",$description);
		 $description=preg_replace("/height=\"\d+\"/","height=\"".$height."\"",$description);
	       }
	       $description=preg_replace("/src=\"\//","src=\"".$site."\/",$description);
	     } else {
	       $unsupported_url=1;
	     }
	   }
	   
	  /*
	   **********************************
	   * Unsupported URL to embed
	   **********************************
           */   
	   if($unsupported_url == 1) {
	       return array(GalleryCoreApi::error(ERROR_UNSUPPORTED_FILE_TYPE,__FILE__,__LINE__,
			    "The URL you have supplied is unsupported (check the list of supported URLs, they must match exactly). You tried to embed a video from this url: $url"),null,null);
	   }
	   

	   /*
	   **********************************
	   * Add the video to Gallery
	   **********************************
           */
	   
	   /* Get a local tmp file to save the thumbnail URL to */
	   $tmpDir = $gallery->getConfig('data.gallery.tmp');
	   $tmpFile = $platform->tempnam($tmpDir, 'add');
	   $tmpFile.= ".jpg";
	   	   
	   /* Fetch the thumbnail and save it to a local file */
  	   list ($successfullyCopied, $response, $headers) =
	     GalleryCoreApi::fetchWebFile($thumbnail, $tmpFile, $extraHeaders);
	   if (!$successfullyCopied) {
	       return array(GalleryCoreApi::error(ERROR_STORAGE_FAILURE,__FILE__,__LINE__,
			    "Unable to copy thumbnail from url: $thumbnail - $response"),null,null);
	   }
	   
	   /* Obtain the mimeType of the thumbnail */
	   list ($ret, $mimeType) = GalleryCoreApi::getMimeType($tmpFile);
	   
	   /* Set the filename for the item we want to add */
	   $fileName = $title;
	   $fileName = preg_replace("/\s+/","_",$fileName);
	   $fileName = preg_replace("/'/","",$fileName);
	   $fileName = preg_replace("/\"/","",$fileName);
	   $fileName = preg_replace("/&#\d+;/","",$fileName);

	   /* Wordwrap the description or summary as appropriate */
	   $description_summary = $summary;

	   if(strcmp($wordwrapSummary,"0")) {
	     $summary = wordwrap($summary,$wordwrapSummary,"<br>",FALSE);
	   }

	   if(strcmp($wordwrapDescription,"0")) {
	     $description_summary = wordwrap($description_summary,$wordwrapDescription,"<br>",FALSE);
	   }
	   
	   /* Tack on the summary to the end of the description */
	   $description.= "<br>$description_summary";

	   /* Add centering tags to the description if requested */
	   if(!strcmp($centerVideo,"true")) {
	     $description = "<center>" . $description . "</center>";
	   }
	   
	   /* General debug output */
	   if(!strcmp($debugOutput,"true")) {
	       print "\n<h2>Item Parameters</h2>\n";
	       print "url: $url <br>\n";
  	       print "title: $title <br>\n";
	       print "thumbnailUrl: $thumbnail <br><img src=\"".$thumbnail."\"> <br>\n";
	       print "summary: <p>$summary</p>\n";
	       print "description: <p>$description</p>\n";
	       print "thumbnail: $tmpFile <br>\n";
	       print "mimeType: $mimeType <br>\n";
	       print "fileName: $fileName <br>\n";
	       print "width: $width <br>\n";
	       print "height: $height <br>\n";
	       print "<hr>";
	   }

	   /* Resize the thumbnail image to the size indicated by our album */
	   $debugString="";
	   list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation($mimeType, 'resize');
	   if (!$ret) {
	       $debugString.="Checking to see if a toolkit that can resize images was found. <br>";
	       if (isset($toolkit)) {
		   $newTmpFile = $platform->tempnam($tmpDir, 'add');
		   $newTmpFile.= ".jpg";
		   $thumbnailSize = 150;
		   list ($ret, $preferences) =
		     GalleryCoreApi::fetchDerivativePreferencesForItem($item->getId());
		   if(!$ret) {
		       foreach ($preferences as $preference) {
			 if (preg_match('/thumbnail\|(\d+)/',
					$preference['derivativeOperations'], $matches)) {
			     $thumbnailSize = $matches[1];
			     $debugString.="Found thumbnail size in album preferences: $thumbnailSize <br>";
			     break;
			 }
		       }
		   }
		   
		   // Obtain the width and height of the original thumbnail, finding out it's ratio,
		   // and using that ratio when determining the width of the video below.
		   // Example: youtube: 130x97 google: 160x120 metacafe: 90x76 yahoo: 100x70
		   // Set the thumbnailHeight to the current thumbnailSize
		   // Set the thumbnailWidth to the appropriate size based on the thumbnailHeight * ratio
		   $image_data = @getimagesize($tmpFile);
		   if(!$image_data) {
		     return array(GalleryCoreApi::error(ERROR_STORAGE_FAILURE,__FILE__,__LINE__,
			    "Unable to retrieve thumbnail dimensions for: $tmpFile"),null,null);
		   }
		   $ratio = $image_data[0] / $image_data[1];
		   $debugString.="Using ratio from original thumbnail of $ratio. ";
		   $thumbnailHeight=$thumbnailSize;
		   $thumbnailWidth=round($thumbnailHeight * $ratio);
		   $debugString.="Resizing thumbnail image to $thumbnailWidth x $thumbnailHeight: $tmpFile -> $newTmpFile <br>";
		   $toolkit->performOperation($mimeType, 'resize', $tmpFile, $newTmpFile,
					      array($thumbnailWidth,$thumbnailHeight));
	       }
	   }
	   if(!strcmp($debugOutput,"true")) {
	       print "\n<h2>Thumbnail Resize</h2>\n";
	       print "$debugString\n";
	       print "<hr>\n";
	   }

	   /* Watermark the video thumbnail image if indicated by our parameter */
	   if(!strcmp($watermarkVideos,"true")) {
	     
	     /*
	      * If no watermarkImage parameter is set, set it to a default value.
	      */
	     if(preg_match("/\w+/", $watermarkImage)) {
	       $watermarkImage = $watermarkImage;
	     } else {
	       $watermarkImage = $gallery2_url.$gallery2_video_watermark;
	     }
	     
	     /* Get the watermark Image Extension */
	     preg_match('/\.(...)$/', $watermarkImage, $matches);
	     $watermarkExt=$matches[1];
	     
	     /*
	      * Check to make sure the URL to the watermark image file is valid
	      * (That the file exists at the URL given). Skip this part if it is a local path.
	      */
	     if(!preg_match("/^\//", $watermarkImage)) {
	       list ($successfullyCopied, $response, $headers) =
		 $this->fetchWebFileHeaders($watermarkImage, $extraHeaders);
	       if (!$successfullyCopied) {
		 return array(GalleryCoreApi::error(ERROR_BAD_PATH,__FILE__,__LINE__,
			"Unable to find the watermark image at url: $watermarkImage - $response"),NULL,NULL);
	       }
	     
	       /* Download the watermark image to a local file */
	       $tmpDir = $gallery->getConfig('data.gallery.tmp');
	       $watermark = $platform->tempnam($tmpDir, 'wmk_img_');
	       $watermark.= "." . $watermarkExt;
	       list ($successfullyCopied, $response, $headers) =
		 GalleryCoreApi::fetchWebFile($watermarkImage, $watermark, $extraHeaders);
	       if (!$successfullyCopied) {
		 return array(GalleryCoreApi::error(ERROR_STORAGE_FAILURE,__FILE__,__LINE__,
			"Unable to copy watermark image from url: $watermarkImage - $response"),null,null);
	       }
	     } else {
	       $tmpDir = $gallery->getConfig('data.gallery.tmp');
	       $watermark = $platform->tempnam($tmpDir, 'wmk_img_');
	       $watermark.= "." . $watermarkExt;
	       if(!$platform->copy($watermarkImage, $watermark)) {
		 return array(GalleryCoreApi::error(ERROR_STORAGE_FAILURE,__FILE__,__LINE__,
			"Unable to copy watermark image from path: $watermarkImage"),null,null);
	       }
	     }
	     
	     /* See if there is a toolkit installed that can perform a composite operation */
	     list ($ret, $toolkit) = GalleryCoreApi::getToolkitByOperation($mimeType, 'composite');
	     if ($ret) {
	       return array($ret->wrap(__FILE__, __LINE__,
		      "Unable to locate a toolkit module to perform the 'composite' watermark operation"), null);
	     }

	     /* Make sure we can access the toolkit found */
	     if (!isset($toolkit)) {
	       return array(GalleryStatus::error(ERROR_PERMISSION_DENIED, __FILE__, __LINE__,
		      "Unable to access the toolkit module to perform the 'composite' watermark operation"), null);
	     }
	     
	     /* Get the image dimensions of the thumbnail */
	     $image_data = @getimagesize($newTmpFile);
	     if(!$image_data) {
	       return array(GalleryCoreApi::error(ERROR_STORAGE_FAILURE,__FILE__,__LINE__,
		      "Unable to retrieve thumbnail dimensions for: $tmpFile"),null,null);
	     }
	     $thumbnailWidth = $image_data[0];
	     $thumbnailHeight = $image_data[1];

	     /* Get the image dimensions of the watermark */
	     $image_data = @getimagesize($watermark);
	     if(!$image_data) {
	       return array(GalleryCoreApi::error(ERROR_STORAGE_FAILURE,__FILE__,__LINE__,
		      "Unable to retrieve watermark dimensions for: $tmpFile"),null,null);
	     }
	     $watermarkWidth = $image_data[0];
	     $watermarkHeight = $image_data[1];

	     /* Obtain the mimeType of the watermark */
	     list ($ret, $watermarkMimeType) = GalleryCoreApi::getMimeType($watermark);

	     /* Remove the gallery base path from the watermark image path */
	     /* This has to be done to satisfy the argument requirement for the toolkit operation */
	     $dataDir = $gallery->getConfig('data.gallery.base');
	     $dataDir=preg_replace("/\//","\\/",$dataDir);
	     if(preg_match("/$dataDir(.*)/", $watermark, $matches)) { 
	       $watermark = $matches[1];
	     }
	     
	     /* General debug output */
	     if(!strcmp($debugOutput,"true")) {
	       print "\n<h2>Watermark Operation</h2>\n";
	       print "watermarkImage: $watermarkImage <br>\n";
	       print "watermarkTmpImage: $watermark <br>\n";
	       print "watermarkMimeType: $watermarkMimeType <br>\n";
	       print "watermarkWidth: $watermarkWidth <br>\n";
	       print "watermarkHeight: $watermarkHeight <br>\n";
	       print "watermarkedWidth: $thumbnailWidth <br>\n";
	       print "watermarkedHeight: $thumbnailHeight <br>\n";
	     }

	     /* Apply the watermark image to the thumbnail */
	     $tmpFile = $platform->tempnam($tmpDir, 'wmk_');
	     list ($ret, $mimeType) = $toolkit->performOperation(
								 $mimeType, 'composite', $newTmpFile, $tmpFile,
								 array($watermark, $watermarkMimeType,
								       $watermarkWidth, $watermarkHeight,
								       $watermarkAlignment, 0, 0));
	     
	     /* Check the return code of the composite operation */
 	     if ($ret) {
	       return array($ret->wrap(__FILE__, __LINE__,
				       "Unable to apply watermark to the video thumbnail image"), null);
	     }

	     if(!strcmp($debugOutput,"true")) {
	       print "\nwatermarked Image: $tmpFile <br>";
	       print "<hr>";
	     }
		       
	     /* Update the path of our thumbnail to point to the new watermarked thumbnail instead */
	     $newTmpFile = $tmpFile;
	     
	   }
	   
	   /* Make the gallery2 call to add this item to the album */
	   list ($ret, $newItem) = GalleryCoreApi::addItemToAlbum($newTmpFile,
								  $fileName,
								  $title,
								  $summary,
								  $description,
								  $mimeType,
								  $item->getId());
	   
	   if ($ret) {
	       return array($ret, null, null);
	   }
	   
	   $status['addedFiles'][] = array('fileName' => $url,
					   'id' => $newItem->getId(),
					   'warnings' => array());

	   if(!strcmp($debugOutput,"true")) {
	       print "\n<br><br><h2>Video Successfully Added to your Gallery Album</h2><br><br><hr>";
	   }
       }
       
       /* Keep the tmpfiles around if we are in debug mode. Otherwise, remove them. */
       if(!strcmp($debugOutput,"false")) {
	 @$platform->unlink($tmpFile);
	 @$platform->unlink($newTmpFile);
       }
       
   }
   
   return array(null, $error, $status);
 }

 /**
  * A simple function to resolve the value of a parameter from
  * the default or override value if it exists, or set it
  * to the default passed as an argument.
  */
 function getParameter($params, $name, $default="") {
   if(isset($params['override'][$name])) {
     /* print "override $name = ".$params['override'][$name]."<br>"; */
     return($params['override'][$name]);
   } else if(isset($params['default'][$name])) {
     /* print "default $name = ".$params['default'][$name] ."<br>"; */
     return($params['default'][$name]);
   } else {
     /* print "$name = $default <br>"; */
     return($default);
   }
 }

 /**
  * A simple function to get the headers only (no body) for a given URL
  * This was taken from GalleryCoreApi::requestWebPage
  */
 function fetchWebFileHeaders($url, $requestHeaders=array()) {
     global $gallery;
     
     $requestMethod='GET';
     $requestBody='';
     
     /* Convert illegal characters */
     $url = str_replace(' ', '%20', $url);
     
     /* Unescape ampersands, since if the URL comes from form input it will be escaped */
     $url = str_replace('&amp;', '&', $url);

     $platform =& $gallery->getPlatform();
     
     $urlComponents = parse_url($url);
     if (empty($urlComponents['port'])) {
         $urlComponents['port'] = 80;
     }
     if (empty($urlComponents['path'])) {
         $urlComponents['path'] = '/';
     }

     $handle = @$platform->fsockopen(
				     $urlComponents['host'], $urlComponents['port'], $errno, $errstr, 5);
     if (empty($handle)) {
         $gallery->debug("Error $errno: '$errstr' requesting $url");
         return array(null, null, null);
     }
     
     $requestUri = $urlComponents['path'];
     if (!empty($urlComponents['query'])) {
         $requestUri .= '?' . $urlComponents['query'];
     }
     $headerLines = array('Host: ' . $urlComponents['host']);
     foreach ($requestHeaders as $key => $value) {
         $headerLines[] = $key . ': ' . $value;
     }
     
     $success = $platform->fwrite($handle, sprintf("%s %s HTTP/1.0\r\n%s\r\n\r\n%s",
						   $requestMethod,
						   $requestUri,
						   implode("\r\n", $headerLines),
						   $requestBody));
     if (!$success) {
         /* Zero bytes written or false was returned */
         $gallery->debug(
			 "fwrite failed in requestWebPage($url)" . ($success === false ? ' - false' : ''));
	 return array(null, null, null);
     }
     $platform->fflush($handle);
     
     
     /*
      * Read the status line.  fgets stops after newlines.  The first line is the protocol
      * version followed by a numeric status code and its associated textual phrase.
      */
     $responseStatus = trim($platform->fgets($handle, 4096));
     if (empty($responseStatus)) {
         $gallery->debug('Empty http response code, maybe timeout');
         return array(null, null, null);
     }
     
     /* Read the headers */
     $responseHeaders = array();
     while (!$platform->feof($handle)) {
         $line = trim($platform->fgets($handle, 4096));
         if (empty($line)) {
	     break;
	 }
       
	 /* Normalize the line endings */
	 $line = str_replace("\r", '', $line);
	 
	 list ($key, $value) = explode(':', $line, 2);
	 if (isset($responseHeaders[$key])) {
	     if (!is_array($responseHeaders[$key])) {
	       $responseHeaders[$key] = array($responseHeaders[$key]);
	     }
	     $responseHeaders[$key][] = trim($value);
	 } else {
	     $responseHeaders[$key] = trim($value);
	 }
     }
     $platform->fclose($handle);

     if(preg_match("/Not found/i", $responseStatus)) {
         $success = 0;
     }

     //print "success: $success <br>responseStatus: $responseStatus <br>responseHeaders: $responseHeaders <br>";
     
     return array($success, $responseStatus, $responseHeaders);
 }
 
 /**
  * @see ItemAdd:loadTemplate
  */
 function loadTemplate(&$template, &$form, $item) {
    global $gallery;
    
    if ($form['formName'] != 'ItemAddEmbedVideo') {
        /* First time around, load the form with item data */
        $form['webPage'] = '';
	$form['formName'] = 'ItemAddEmbedVideo';
    }
    
    $session =& $gallery->getSession();
    
    $template->setVariable('ItemAddEmbedVideo', $ItemAddEmbedVideo);
    
    return array(null,
		 'modules/embedvideo/templates/ItemAddEmbedVideo.tpl',
		 'modules_embedvideo');
 }
 
 /**
  * @see ItemAddPlugin::getTitle
  */
 function getTitle() {
    list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'embedvideo');
    if ($ret) {
        return array($ret, null);
    }
    
    return array(null, $module->translate('Embed Video'));
 }
 
}	
?>
