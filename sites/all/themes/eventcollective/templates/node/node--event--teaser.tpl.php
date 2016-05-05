<a href="<?php print $node_url; ?>">
  <div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
    <div class="content"<?php print $content_attributes; ?>>
      <?php
        // print render($content);
        // print $author_pic;
        // print $task_icon;
        // print $task_points;
        // print $difficulty;
        // print $task_meta;
      ?>
    </div>
  </div>
  <div class="event-meta clearfix">
    <?php print render($title_prefix); ?>
    <?php if (!$page): ?>
      <h2<?php print $title_attributes; ?> class="text-center"><?php print $title; ?></h2>
    <?php endif; ?>
    <?php print render($title_suffix); ?>
  </div>
</a>
