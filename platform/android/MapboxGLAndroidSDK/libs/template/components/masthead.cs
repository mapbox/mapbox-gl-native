<?cs def:custom_masthead() ?>
<div id="header">
    <div id="headerLeft">
      <a class="fl mb-logo white" href="/"></a>
    </div>
    <div id="headerRight">
      <?cs call:default_search_box() ?>
      <?cs if:reference && reference.apilevels ?>
        <?cs call:default_api_filter() ?>
      <?cs /if ?>
    </div>
</div><!-- header -->
<?cs /def ?>
