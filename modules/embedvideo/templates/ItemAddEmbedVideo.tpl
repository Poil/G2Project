{*
 * $Revision: 1 $
 * If you want to customize this file, do not edit it directly since future upgrades
 * may overwrite it.  Instead, copy it into a new directory called "local" and edit that
 * version.  Gallery will look for that file first and use it if it exists.
 *}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Import embedded videos into Gallery from another website.  Enter a URL below to a web page anywhere on the net and Gallery will add an embedded link to that file."}
  </p>

  {if empty($form.webPageUrls)}
    <h4> {g->text text="URL"} </h4>

    <input type="text" size="80"
     name="{g->formVar var="form[webPage][URL]"}" value="{$form.webPage}"/>

    <br/>

    <br> 
    <b><u>Supported Video Sites:</u></b> <br>
    <b>YouTube:</b> http://www.youtube.com/watch?v=xxxxxxxxxxx<br>
    <b>YouTube:</b> http://www.youtube.com/v/xxxxxxxxxxx<br>
    <b>GoogleVideo:</b> http://video.google.com/videoplay?docid=xxxxxxxxxxx<br>
    <b>YahooVideo:</b> http://video.yahoo.com/video/play?vid=xxxxxx<br>
    <b>Metacafe:</b> http://www.metacafe.com/watch/xxxxxx/xxxxxx<br>
    <b>Revver:</b> http://www.revver.com/watch/xxxxxx<br>
    <b>Dailymotion:</b> http://www.dailymotion.com/video/xxxxxxxxxxx<br>
    <b>Divshare:</b> http://www.divshare.com/download/xxxxxxxxx<br>
    <b>Stickam:</b> http://www.stickham.com/editMediaComment.do?method=load&mId=xxxxxxxxx<br>
    <b>Myspace1:</b> http://vids.myspace.com/index.cfm?fuseaction=vids.individual&VideoID=xxxxxxxx<br>
    <b>Myspace2:</b> http://myspacetv.com/index.cfm?fuseaction=vids.individual&videoid=xxxxxxxx<br>
    <b>Vimeo:</b> http://www.vimeo.com/xxxxxx<br>
    <b>Animoto:</b> http://www.animoto.com/play/xxxxxxxxxxxxxxxxxxxxxx<br>
    <b>Generic:</b> Any webpage with an '&lt;object ... &lt;embed ...' style video on its page<br>
    <br>
    <b><u>Supported File Types:</u></b><br>
    <b>FlashVideo (.flv):</b> http://aaa.bbb.ccc/video.flv<br>
    <b>FlashVideo (.swf):</b> http://aaa.bbb.ccc/video.swf<br>
    <b>Mpeg4 with an external FLV players (.mp4):</b> http://aaa.bbb.ccc/video.mp4<br>
    <br>

    <a href="http://codex.gallery2.org/Gallery2:Modules:embedvideo">Embed Video Module Help Page</a>

  </div>

  <div class="gbBlock">

    {capture name="submitButtons"}
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][addEmbedVideoPage]"}" value="{g->text text="Add Embedded Video"}"/>
    {/capture}
  {/if} {* {if !empty($form.webPageUrls)} *}
</div>


<div class="gbBlock gcBackground1">
  {$smarty.capture.submitButtons}
</div>


