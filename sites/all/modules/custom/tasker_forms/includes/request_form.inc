<?php
// The popup form to allow KW's to inform the Ops team what days they can work from their invite
function tasker_forms_request_form($form, &$form_state){
  global $user;
  $nw = entity_metadata_wrapper('node', arg(1));
  $form_state['nid'] = arg(1);
  foreach ($nw->field_t_users->getIterator() as $k => $v) {
    if($v->getIdentifier() == $user->uid){
      $requested = true;
      break;
    }
  }
  $desc = !isset($requested) ? t('You are submitting a request to the task owner - to add a note to the request, leave a message below!') : t('You are <strong>removing</strong> your task request - to add a note, leave a message below!');
  $form['form_description'] = array(
    '#markup' => '<p>'.$desc.'</p>',
  );

  $form['msg'] = array(
    '#type' => 'textarea',
    '#title' => t('Message:'),
    '#description' => t('Attach a message to your task request.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => !isset($requested) ? t('submit request') : t('remove request'),
    '#attributes' => array(
      'class' => !isset($requested) ? array('btn-success') : array('btn-danger'),
      'data-loading-text' => array("Saving.."),
      'data-complete-text' => array('Saved!'),
    ),
  );
  return $form;
}

function tasker_forms_request_form_submit($form, &$form_state){
  global $user;
  $nw = entity_metadata_wrapper('node', $form_state['nid']);

  if(sizeof($nw->field_t_users->value()) > 0){
    // dpm($nw->field_t_users->value());
    $flag = 0;
    foreach ($nw->field_t_users->getIterator() as $k => $u) {
      if($u->getIdentifier() == $user->uid){
        $nw->field_t_users[$k] = NULL;
        $flag = 1;
        break;
      }
    }
    if($flag == 0){
      $nw->field_t_users->offsetSet(sizeof($nw->field_t_users->value()), $user->uid);
    }
  } else {
    $nw->field_t_users->offsetSet(0, $user->uid);
  }
  $nw->save();
  drupal_set_message(t('Your request has been sent to the task owner - you will receive notifcation if accepted!'), 'status', FALSE);
}
