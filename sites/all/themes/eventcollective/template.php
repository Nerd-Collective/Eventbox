<?php
/**
 * @file
 * The primary PHP file for this theme.
 */

/**
 * Implements hook_preprocess_node().
 */
function eventcollective_preprocess_node(&$variables) {
  global $user;
  if($variables['view_mode'] == 'teaser') {
    $variables['theme_hook_suggestions'][] = 'node__' . $variables['node']->type . '__teaser';
    $variables['theme_hook_suggestions'][] = 'node__' . $variables['node']->nid . '__teaser';
  }

  if($variables['type'] == 'event'){
    if($variables['view_mode'] == 'teaser') {
      // dpm_once($variables);
      $nw = entity_metadata_wrapper('node', $variables['nid']);

    }
  }
}

/**
 * Implements hook_preprocess_page().
 */
function eventcollective_preprocess_page(&$variables) {
  // Set navbar to fixed.
  // navbar navbar-default navbar-fixed-top
  $variables['navbar_classes_array'][1] = 'navbar-fixed-top';
  $variables['container_class'] = 'container-fluid';

  if (strpos(current_path(), 'node') !== FALSE || strpos(current_path(), 'agenda') !== FALSE) {
    $variables['content_column_class'] = ' class="col-sm-12 event-page"';
  }
}

/**
 * Implements hook_preprocess_html().
 */
function eventcollective_preprocess_html(&$variables) {
  if (strpos(current_path(), 'node') !== FALSE || strpos(current_path(), 'agenda') !== FALSE) {
    $variables['classes_array'][] = 'event-mode';
  }
}

/**
 * Implements hook_preprocess_region().
 */
function eventcollective_preprocess_region(&$variables) {
  if ($variables['region'] == 'content') {
    if (arg(0) == 'user') {
      $variables['classes_array'][] = 'row';
    }
  }
}

/**
 * Quick function to dpm only one call from a loop etc.
 */
function dpm_once($input, $name = NULL, $type = 'status') {
  $backtrace = debug_backtrace();
  $caller = array_shift($backtrace);
  $executed = &drupal_static(__FUNCTION__ . $caller['file'] . $caller['line'], FALSE);
  if (!$executed) {
    $executed = TRUE;
    if (function_exists('dpm')) {
      dpm($input, $name, $type);
    }
  }
}
