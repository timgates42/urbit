import React, { Component } from 'react';
import { Box, Text } from '@tlon/indigo-react';
import { ShipSearch } from '~/views/components/ShipSearch';
import { Formik, Form, FormikHelpers } from 'formik';
import { resourceFromPath } from '~/logic/lib/group';
import { AsyncButton } from '~/views/components/AsyncButton';

export class Writers extends Component {
  render() {
    const { association, groups, contacts, api } = this.props;

    const [,,,name] = association?.['app-path'].split('/');
    const resource = resourceFromPath(association?.['group-path']);

    const onSubmit = async (values, actions) => {
      try {
        const ships = values.ships.map(e => `~${e}`);
        await api.groups.addTag(
          resource,
          { app: 'publish', tag: `writers-${name}` },
          ships
        );
        actions.resetForm();
        actions.setStatus({ success: null });
      } catch (e) {
        console.error(e);
        actions.setStatus({ error: e.message });
      }
    };

    return (
      <Box maxWidth='512px'>
        <Text display='block'>Writers</Text>
        <Text display='block' mt='2' gray>Add additional writers to this notebook</Text>
        <Formik
          initialValues={{ ships: [] }}
          onSubmit={onSubmit}
        >
          <Form>
            <ShipSearch
              groups={groups}
              contacts={contacts}
              id="ships"
              label=""
              maxLength={undefined}
            />
            <AsyncButton width='100%' mt='3' primary>
            Submit
            </AsyncButton>
          </Form>
        </Formik>
      </Box>
    );
  }
}

export default Writers;
