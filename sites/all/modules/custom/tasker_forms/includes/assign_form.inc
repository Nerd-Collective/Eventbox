<?php
// The popup form to allow KW's to inform the Ops team what days they can work from their invite
function tasker_forms_assign_form($form, &$form_state){
  global $user;
  $nw = entity_metadata_wrapper('node', arg(1));
  $form_state['nid'] = arg(1);
  $usrs = false;

  foreach ($nw->field_t_users->getIterator() as $k => $u) {
    $usrs .= theme('task_users', array('nid' => $nw->getIdentifier(), 'modal' => true, 'uid' => $u->getIdentifier()));
  }

  $desc = t('Select the individual(s) you want to complete you task.');
  $form['form_description'] = array(
    '#markup' => '<p>'.$desc.'</p>',
  );
  $form['assignees'] = array(
    '#markup' => '<div class="clearfix field-name-requestees">'.$usrs.'</div>',
  );
  $form['uids'] = array(
    '#type' => 'hidden',
    '#attributes' => array('id'=>array('hidden-uids')),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('assign task'),
    '#attributes' => array(
      'class' => !isset($requested) ? array('btn-success') : array('btn-danger'),
      'data-loading-text' => array("Assigning.."),
      'data-complete-text' => array('Assigned!'),
    ),
  );
  return $form;
}

function tasker_forms_assign_form_submit($form, &$form_state){
  global $user;
  // dpm($form_state['values']);
  $nw = entity_metadata_wrapper('node', $form_state['nid']);
  $uids = explode(',', rtrim($form_state['values']['uids'], ','));

  $prev_users = array();
  foreach ($nw->field_t_users->getIterator() as $key => $u) {
    $prev_users[] = $u->getIdentifier();
  }

  $selected = array_intersect($uids, $prev_users);
  $not_selected = array_diff($prev_users, $uids);

  foreach ($not_selected as $key => $sel) {
    $uw = entity_metadata_wrapper('user', $sel);
    drupal_set_message(t('Bad luck - '.$uw->label().', you haven\'nt been chosen this t!'), 'status', FALSE);
  }

  $nw->field_t_users->set(NULL);
  $i=0;
  foreach ($selected as $key => $sel) {
    $uw = entity_metadata_wrapper('user', $sel);
    $nw->field_t_users->offsetSet($i, $uw->getIdentifier());
    $i++;
    drupal_set_message(t('Email to send to: - '.$uw->label()), 'status', FALSE);
  }
  $nw->field_t_status->set('progress');
  $nw->save();
}
